#include <catch2/catch.hpp>

#include "wondermake-utility/UniqueFunction.h"

#include <functional>

static bool MemoryError = false;
static size_t MemoryUsage = 0;

static void ResetMemoryCounter()
{
	MemoryError = false;
	MemoryUsage = 0;
}
static bool CheckMemoryCounter()
{
	return !MemoryError
		&& MemoryUsage == 0;
}

template<typename TType>
struct TestUniqueFunctionAllocator
{
	template<typename... TArgs>
	static [[nodiscard]] TType* Allocate(TArgs&&... aArgs)
	{
		MemoryUsage += sizeof(TType);

		return new TType(std::forward<TArgs>(aArgs)...);
	}
	static void Deallocate(TType* const aObject)
	{
		if (MemoryUsage < sizeof(TType)
			|| !aObject)
		{
			MemoryError = true;
			MemoryUsage = 0;

			return;
		}
		
		MemoryUsage -= sizeof(TType);
		delete aObject;
	}
};

#define WM_TEST_MEMORY_BEGIN ResetMemoryCounter();
#define WM_TEST_MEMORY_END CHECK(CheckMemoryCounter());

template<typename TFunction>
using TestUniqueFunction = UniqueFunction<TFunction, TestUniqueFunctionAllocator>;

struct SematicsData
{
	int CounterConstruct		= 0;
	int CounterConstructCopy	= 0;
	int CounterConstructMove	= 0;
	int CounterDeconstruct		= 0;
	int CounterAssignCopy		= 0;
	int CounterAssignMove		= 0;
};

struct SematicsCounter
{
	SematicsCounter(SematicsData& aData) noexcept
		: Data(aData)
	{
		++Data.CounterConstruct;
	}
	SematicsCounter(const SematicsCounter& aRhs) noexcept
		: Data(aRhs.Data)
	{
		++Data.CounterConstructCopy;
	}
	SematicsCounter(SematicsCounter&& aRhs) noexcept
		: Data(aRhs.Data)
	{
		++Data.CounterConstructMove;
	}
	~SematicsCounter() noexcept
	{
		++Data.CounterDeconstruct;
	}

	void operator=(const SematicsCounter& aRhs) noexcept
	{
		Data = aRhs.Data;

		++Data.CounterAssignCopy;
	}
	void operator=(SematicsCounter&& aRhs) noexcept
	{
		Data = aRhs.Data;

		++Data.CounterAssignMove;
	}

	SematicsData& Data;
};

void StaticTest_UniqueFunction()
{
	static_assert(sizeof(TestUniqueFunction<void()>) == 64);
	static_assert(sizeof(TestUniqueFunction<void(int)>) == 64);
	static_assert(sizeof(TestUniqueFunction<int()>) == 64);
	static_assert(sizeof(TestUniqueFunction<int(int)>) == 64);
}

TEST_CASE("UniqueFunction can be constructed and invoked", "[UniqueFunction]")
{
	struct TestCallable
		: public SematicsCounter
	{
		TestCallable(SematicsData& aData, int& aCounter) noexcept
			: SematicsCounter(aData)
			, Counter(aCounter)
		{}

		void operator()() noexcept
		{
			++Counter;
		}
		void operator()(int aNum) noexcept
		{
			Counter += aNum;
		}

		int& Counter;
	};

	struct TestCallableBig
		: public TestCallable
	{
		using TestCallable::TestCallable;

		char Filler[1024];
	};

	WM_TEST_MEMORY_BEGIN
	{
		int counter = 0;

		SECTION("Standard Callables")
		{
			SECTION("Lambda void()")
			{
				TestUniqueFunction<void()> func([&counter]() noexcept
					{
						++counter;
					});

				CHECK(counter == 0);

				std::move(func)();

				CHECK(counter == 1);
			}
			SECTION("Lambda void(int)")
			{
				TestUniqueFunction<void(int)> func([&counter](int aNum)
					{
						counter += aNum;
					});

				CHECK(counter == 0);

				std::move(func)(4);

				CHECK(counter == 4);
			}
		
			SECTION("std::function void()")
			{
				TestUniqueFunction<void()> func(std::function<void()>([&counter]()
					{
						++counter;
					}));

				CHECK(counter == 0);

				std::move(func)();

				CHECK(counter == 1);
			}
			SECTION("std::function void(int)")
			{
				TestUniqueFunction<void(int)> func(std::function<void(int)>([&counter](int aNum)
					{
						counter += aNum;
					}));

				CHECK(counter == 0);

				std::move(func)(4);

				CHECK(counter == 4);
			}
		}

		SECTION("Custom Callables")
		{
			SematicsData data;

			SECTION("TestCallable void()")
			{
				TestUniqueFunction<void()> func(TestCallable(data, counter));

				CHECK(counter == 0);

				std::move(func)();

				CHECK(counter == 1);
			}
			SECTION("TestCallable void(int)")
			{
				TestUniqueFunction<void(int)> func(TestCallable(data, counter));

				CHECK(counter == 0);

				std::move(func)(4);

				CHECK(counter == 4);
			}

			SECTION("TestCallableBig void()")
			{
				TestUniqueFunction<void()> func(TestCallableBig(data, counter));

				CHECK(counter == 0);

				std::move(func)();

				CHECK(counter == 1);
			}
			SECTION("TestCallableBig void(int)")
			{
				TestUniqueFunction<void(int)> func(TestCallableBig(data, counter));

				CHECK(counter == 0);

				std::move(func)(4);

				CHECK(counter == 4);
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 1);
			CHECK(data.CounterDeconstruct == 2);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}
	}
	WM_TEST_MEMORY_END
}

TEST_CASE("UniqueFunction arguments and return values are forwarded properly", "[UniqueFunction]")
{
	WM_TEST_MEMORY_BEGIN
	{
		SematicsData data;

		SECTION("void(Counter&)")
		{
			{
				TestUniqueFunction<void(SematicsCounter&)> func([](auto&&) {});
				SematicsCounter counter(data);

				std::move(func)(counter);
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 0);
			CHECK(data.CounterDeconstruct == 1);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}
		SECTION("void(Counter)")
		{
			{
				TestUniqueFunction<void(SematicsCounter)> func([](auto&&) {});
				SematicsCounter counter(data);

				std::move(func)(std::move(counter));
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 1);
			CHECK(data.CounterDeconstruct == 2);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}
		SECTION("void(Counter&, Counter)")
		{
			{
				TestUniqueFunction<void(SematicsCounter&, SematicsCounter)> func([](auto&&, auto&&) {});
				SematicsCounter counterA(data);
				SematicsCounter counterB(data);

				std::move(func)(counterA, std::move(counterB));
			}

			CHECK(data.CounterConstruct == 2);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 1);
			CHECK(data.CounterDeconstruct == 3);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}

		SECTION("Counter()")
		{
			{
				TestUniqueFunction<SematicsCounter()> func([&data]() { return SematicsCounter(data); });

				SematicsCounter counter = std::move(func)();
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 0);
			CHECK(data.CounterDeconstruct == 1);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}

	}
	WM_TEST_MEMORY_END
}

TEST_CASE("UniqueFunction callables are copied properly when the UniqueFunction is moved.", "[UniqueFunction]")
{
	WM_TEST_MEMORY_BEGIN
	{
		SematicsData data;

		SECTION("Callable")
		{
			{
				// Construct
				auto counter = SematicsCounter(data);

				// ConstructMove
				auto callable = [counter = std::move(counter)]() {};

				// ConstructMove
				TestUniqueFunction<void()> funcA(std::move(callable));

				// ConstructMove
				auto funcB = std::move(funcA);
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 3);
			CHECK(data.CounterDeconstruct == 4);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}

		SECTION("Big Callable")
		{
			struct BigObject
			{
				char Filler[1024];
			};

			{
				// Construct
				SematicsCounter counter(data);

				// ConstructMove
				auto callable = [counter = std::move(counter), bigObject = BigObject()]() {};

				// ConstructMove
				TestUniqueFunction<void()> funcA(std::move(callable));

				// Object not stored locally, no movement done.
				auto funcB = std::move(funcA);
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 2);
			CHECK(data.CounterDeconstruct == 3);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}
	}
	WM_TEST_MEMORY_END
}

TEST_CASE("UniqueFunction is able to pass exceptions without side-effects", "[UniqueFunction]")
{
	WM_TEST_MEMORY_BEGIN
	{
		SematicsData data;
	
		SECTION("Invoke")
		{
			{
				SematicsCounter counter(data);

				TestUniqueFunction<void()> func([counter = std::move(counter)]()
					{
						throw std::exception();
					});

				REQUIRE_THROWS(std::move(func)());
			}

			CHECK(data.CounterConstruct == 1);
			CHECK(data.CounterConstructCopy == 0);
			CHECK(data.CounterConstructMove == 2);
			CHECK(data.CounterDeconstruct == 3);
			CHECK(data.CounterAssignCopy == 0);
			CHECK(data.CounterAssignMove == 0);
		}
	}
	WM_TEST_MEMORY_END
}
