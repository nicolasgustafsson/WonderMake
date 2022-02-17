#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/AnyExecutor.h"

#include "wondermake-utility/Typedefs.h"

class ExecutorCallableMock
{
public:
	MOCK_METHOD(void, Executed, ());

	auto GetCallable()
	{
		return [this]()
		{
			Executed();
		};
	}
};

class ExecutorMock
{
public:
	MOCK_METHOD(void, Execute, (Closure));

	auto GetExecutor()
	{
		class InternalExecutor
		{
		public:
			InternalExecutor(ExecutorMock& aExecutorMock) noexcept
				: myExecutor(aExecutorMock)
			{}
			InternalExecutor(const InternalExecutor&) noexcept = default;

			void Execute(Closure&& aClosure) const
			{
				myExecutor.Execute(std::move(aClosure));
			}

		private:
			ExecutorMock& myExecutor;

		};

		return InternalExecutor(*this);
	}
};

TEST(AnyExecutorTests, executor_execute_does_nothing_when_no_executor_is_passed)
{
	StrictMock<ExecutorCallableMock> callableMock;
	AnyExecutor anyExecutor;

	anyExecutor.Execute(callableMock.GetCallable());
}

TEST(AnyExecutorTests, executor_execute_calls_passed_executors_execute)
{
	StrictMock<ExecutorMock> executorMock;
	StrictMock<ExecutorCallableMock> callableMock;

	AnyExecutor anyExecutor(executorMock.GetExecutor());

	EXPECT_CALL(executorMock, Execute);

	anyExecutor.Execute(callableMock.GetCallable());
}

TEST(AnyExecutorTests, executor_execute_calls_passes_correct_callable)
{
	StrictMock<ExecutorCallableMock> callableMock;
	InlineExecutor executor;

	AnyExecutor anyExecutor(executor);

	EXPECT_CALL(callableMock, Executed);

	anyExecutor.Execute(callableMock.GetCallable());
}

TEST(AnyExecutorTests, copied_executor_calls_passed_executors_execute)
{
	StrictMock<ExecutorMock> executorMock;
	StrictMock<ExecutorCallableMock> callableMock;

	AnyExecutor anyExecutor(executorMock.GetExecutor());

	AnyExecutor otherAnyExecutor(anyExecutor);

	EXPECT_CALL(executorMock, Execute);

	otherAnyExecutor.Execute(callableMock.GetCallable());
}

TEST(AnyExecutorTests, moved_executor_calls_passed_executors_execute)
{
	StrictMock<ExecutorMock> executorMock;
	StrictMock<ExecutorCallableMock> callableMock;

	AnyExecutor anyExecutor(executorMock.GetExecutor());

	AnyExecutor otherAnyExecutor(std::move(anyExecutor));

	EXPECT_CALL(executorMock, Execute);

	otherAnyExecutor.Execute(callableMock.GetCallable());
}
