#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/Typedefs.h"
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
#define WM_TEST_MEMORY_END EXPECT_TRUE(CheckMemoryCounter());

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

	void Reset()
	{
		CounterConstruct		= 0;
		CounterConstructCopy	= 0;
		CounterConstructMove	= 0;
		CounterDeconstruct		= 0;
		CounterAssignCopy		= 0;
		CounterAssignMove		= 0;
	}
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

class CallableEmbeddedMock
{
public:
	MOCK_METHOD(void,					ExecuteRetVoidArgVoid,		());
	MOCK_METHOD(void,					ExecuteRetVoidArgI32,		(i32));
	MOCK_METHOD(i32,					ExecuteRetI32ArgVoid,		());
	MOCK_METHOD(void,					ExecuteRetVoidArgUnique,	(std::unique_ptr<i32>));
	MOCK_METHOD(std::unique_ptr<i32>,	ExecuteRetUniqueArgVoid,	());
	
	auto GetCallableRetVoidArgVoid()
	{
		return [this]()
		{
			ExecuteRetVoidArgVoid();
		};
	}
	auto GetCallableRetVoidArgI32()
	{
		return [this](i32 aI32)
		{
			ExecuteRetVoidArgI32(aI32);
		};
	}
	auto GetCallableRetI32ArgVoid()
	{
		return [this]()
		{
			return ExecuteRetI32ArgVoid();
		};
	}
	auto GetCallableRetVoidArgUnique()
	{
		return [this](std::unique_ptr<i32> aUnique)
		{
			ExecuteRetVoidArgUnique(std::move(aUnique));
		};
	}
	auto GetCallableRetUniqueArgVoid()
	{
		return [this]()
		{
			return ExecuteRetUniqueArgVoid();
		};
	}

};

class CallableBigMock
	: public CallableEmbeddedMock
{
private:
	char Filler[1024];

};

struct SematicsCounterCallable
	: public SematicsCounter
{
	SematicsCounterCallable(SematicsData& aData, u32& aCountervoid, u32& aCounterI32) noexcept
		: SematicsCounter(aData)
		, CounterVoid(aCountervoid)
		, CounterI32(aCounterI32)
	{}

	void operator()() noexcept
	{
		++CounterVoid;
	}
	void operator()(i32 aNum) noexcept
	{
		++CounterI32;
	}

	u32& CounterVoid;
	u32& CounterI32;
};

TEST(UniqueFunctionTests, uniquefunction_can_be_constructed_and_invoked_with_ret_void_arg_void)
{
	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void()> func(mock.GetCallableRetVoidArgVoid());

		EXPECT_CALL(mock, ExecuteRetVoidArgVoid);

		std::move(func)();
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableBigMock> mock;

		TestUniqueFunction<void()> func(mock.GetCallableRetVoidArgVoid());

		EXPECT_CALL(mock, ExecuteRetVoidArgVoid);

		std::move(func)();
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void()> func(std::function<void()>(mock.GetCallableRetVoidArgVoid()));

		EXPECT_CALL(mock, ExecuteRetVoidArgVoid);

		std::move(func)();
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		SematicsData sematicsData;
		u32 counterVoid	= 0;
		u32 counterI32	= 0;

		{
			TestUniqueFunction<void()> func(SematicsCounterCallable(sematicsData, counterVoid, counterI32));

			EXPECT_EQ(counterVoid,							0);
			EXPECT_EQ(counterI32,							0);
			EXPECT_EQ(sematicsData.CounterConstruct,		1);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		1);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			counterVoid = 0;
			counterI32 = 0;
			sematicsData.Reset();

			std::move(func)();
			
			EXPECT_EQ(counterVoid,							1);
			EXPECT_EQ(counterI32,							0);
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			counterVoid = 0;
			counterI32 = 0;
			sematicsData.Reset();
		}
		
		EXPECT_EQ(counterVoid,							0);
		EXPECT_EQ(counterI32,							0);
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		1);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_can_be_constructed_and_invoked_with_ret_void_arg_i32)
{
	static constexpr i32 dummyValue = 1234;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void(i32)> func(mock.GetCallableRetVoidArgI32());

		EXPECT_CALL(mock, ExecuteRetVoidArgI32(dummyValue));

		std::move(func)(dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableBigMock> mock;

		TestUniqueFunction<void(i32)> func(mock.GetCallableRetVoidArgI32());

		EXPECT_CALL(mock, ExecuteRetVoidArgI32(dummyValue));

		std::move(func)(dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void(i32)> func(std::function<void(i32)>(mock.GetCallableRetVoidArgI32()));

		EXPECT_CALL(mock, ExecuteRetVoidArgI32(dummyValue));

		std::move(func)(dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		SematicsData sematicsData;
		u32 counterVoid	= 0;
		u32 counterI32	= 0;

		{
			TestUniqueFunction<void(i32)> func(SematicsCounterCallable(sematicsData, counterVoid, counterI32));

			EXPECT_EQ(counterVoid,							0);
			EXPECT_EQ(counterI32,							0);
			EXPECT_EQ(sematicsData.CounterConstruct,		1);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		1);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			counterVoid = 0;
			counterI32 = 0;
			sematicsData.Reset();

			std::move(func)(dummyValue);
			
			EXPECT_EQ(counterVoid,							0);
			EXPECT_EQ(counterI32,							1);
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			counterVoid = 0;
			counterI32 = 0;
			sematicsData.Reset();
		}
		
		EXPECT_EQ(counterVoid,							0);
		EXPECT_EQ(counterI32,							0);
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		1);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_can_be_constructed_and_invoked_with_ret_i32_arg_void)
{
	static constexpr i32 dummyValue = 1234;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<i32()> func(mock.GetCallableRetI32ArgVoid());

		EXPECT_CALL(mock, ExecuteRetI32ArgVoid)
			.WillOnce(Return(dummyValue));

		const i32 retVal = std::move(func)();

		EXPECT_EQ(retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableBigMock> mock;

		TestUniqueFunction<i32()> func(mock.GetCallableRetI32ArgVoid());

		EXPECT_CALL(mock, ExecuteRetI32ArgVoid)
			.WillOnce(Return(dummyValue));

		const i32 retVal = std::move(func)();

		EXPECT_EQ(retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<i32()> func(std::function<i32()>(mock.GetCallableRetI32ArgVoid()));

		EXPECT_CALL(mock, ExecuteRetI32ArgVoid)
			.WillOnce(Return(dummyValue));

		const i32 retVal = std::move(func)();

		EXPECT_EQ(retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_can_be_constructed_and_invoked_with_ret_void_arg_unique)
{
	static constexpr i32 dummyValue = 1234;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void(std::unique_ptr<i32>)> func(mock.GetCallableRetVoidArgUnique());

		EXPECT_CALL(mock, ExecuteRetVoidArgUnique)
			.WillOnce([dummyValue = dummyValue](auto aPtr)
				{
					ASSERT_TRUE(aPtr);

					EXPECT_EQ(*aPtr, dummyValue);
				});

		std::move(func)(std::make_unique<i32>(dummyValue));
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableBigMock> mock;

		TestUniqueFunction<void(std::unique_ptr<i32>)> func(mock.GetCallableRetVoidArgUnique());
		
		EXPECT_CALL(mock, ExecuteRetVoidArgUnique)
			.WillOnce([dummyValue = dummyValue](auto aPtr)
				{
					ASSERT_TRUE(aPtr);

					EXPECT_EQ(*aPtr, dummyValue);
				});

		std::move(func)(std::make_unique<i32>(dummyValue));
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<void(std::unique_ptr<i32>)> func(std::function<void(std::unique_ptr<i32>)>(mock.GetCallableRetVoidArgUnique()));

		EXPECT_CALL(mock, ExecuteRetVoidArgUnique)
			.WillOnce([dummyValue = dummyValue](auto aPtr)
				{
					ASSERT_TRUE(aPtr);

					EXPECT_EQ(*aPtr, dummyValue);
				});

		std::move(func)(std::make_unique<i32>(dummyValue));
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_can_be_constructed_and_invoked_with_ret_unique_arg_void)
{
	static constexpr i32 dummyValue = 1234;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<std::unique_ptr<i32>()> func(mock.GetCallableRetUniqueArgVoid());

		EXPECT_CALL(mock, ExecuteRetUniqueArgVoid)
			.WillOnce(Return(ByMove(std::make_unique<i32>(dummyValue))));

		const auto retVal = std::move(func)();

		EXPECT_EQ(*retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN
	{
		StrictMock<CallableBigMock> mock;

		TestUniqueFunction<std::unique_ptr<i32>()> func(mock.GetCallableRetUniqueArgVoid());

		EXPECT_CALL(mock, ExecuteRetUniqueArgVoid)
			.WillOnce(Return(ByMove(std::make_unique<i32>(dummyValue))));

		const auto retVal = std::move(func)();

		EXPECT_EQ(*retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;

	WM_TEST_MEMORY_BEGIN;
	{
		StrictMock<CallableEmbeddedMock> mock;

		TestUniqueFunction<std::unique_ptr<i32>()> func(std::function<std::unique_ptr<i32>()>(mock.GetCallableRetUniqueArgVoid()));

		EXPECT_CALL(mock, ExecuteRetUniqueArgVoid)
			.WillOnce(Return(ByMove(std::make_unique<i32>(dummyValue))));

		const auto retVal = std::move(func)();

		EXPECT_EQ(*retVal, dummyValue);
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_callables_are_moved_properly)
{
	WM_TEST_MEMORY_BEGIN
	{
		SematicsData sematicsData;

		{
			// Construct
			auto counter = SematicsCounter(sematicsData);
			
			EXPECT_EQ(sematicsData.CounterConstruct,		1);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			auto callable = [counter = std::move(counter)]() {};
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			TestUniqueFunction<void()> funcA(std::move(callable));
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			auto funcB = std::move(funcA);
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		1);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();
		}
		
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		3);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
	
	WM_TEST_MEMORY_BEGIN
	{
		struct BigObject
		{
			char Filler[1024];
		};

		SematicsData sematicsData;

		{
			// Construct
			auto counter = SematicsCounter(sematicsData);
			
			EXPECT_EQ(sematicsData.CounterConstruct,		1);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			auto callable = [counter = std::move(counter), bigObject = BigObject()]() {};
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			TestUniqueFunction<void()> funcA(std::move(callable));
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	1);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();

			// ConstructMove
			auto funcB = std::move(funcA);
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);

			sematicsData.Reset();
		}
		
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		3);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
}

TEST(UniqueFunctionTests, uniquefunction_passes_exceptions_without_side_effects)
{
	WM_TEST_MEMORY_BEGIN
	{
		SematicsData sematicsData;

		{
			auto counter = SematicsCounter(sematicsData);
			
			auto callable = [counter = std::move(counter)]()
			{
				throw std::exception();
			};
			
			TestUniqueFunction<void()> func(std::move(callable));

			sematicsData.Reset();

			EXPECT_ANY_THROW(std::move(func)());
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);
		}
		
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		3);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
	
	WM_TEST_MEMORY_BEGIN
	{
		struct BigObject
		{
			char Filler[1024];
		};

		SematicsData sematicsData;

		{
			auto counter = SematicsCounter(sematicsData);

			auto callable = [counter = std::move(counter), bigObject = BigObject()]()
			{
				throw std::exception();
			};
			
			TestUniqueFunction<void()> func(std::move(callable));

			sematicsData.Reset();

			EXPECT_ANY_THROW(std::move(func)());
			
			EXPECT_EQ(sematicsData.CounterConstruct,		0);
			EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
			EXPECT_EQ(sematicsData.CounterConstructMove,	0);
			EXPECT_EQ(sematicsData.CounterDeconstruct,		0);
			EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
			EXPECT_EQ(sematicsData.CounterAssignMove,		0);
		}
		
		EXPECT_EQ(sematicsData.CounterConstruct,		0);
		EXPECT_EQ(sematicsData.CounterConstructCopy,	0);
		EXPECT_EQ(sematicsData.CounterConstructMove,	0);
		EXPECT_EQ(sematicsData.CounterDeconstruct,		3);
		EXPECT_EQ(sematicsData.CounterAssignCopy,		0);
		EXPECT_EQ(sematicsData.CounterAssignMove,		0);
	}
	WM_TEST_MEMORY_END;
}
