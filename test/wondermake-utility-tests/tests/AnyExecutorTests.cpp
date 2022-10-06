#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility-tests/ExecutorMock.h"

#include "wondermake-utility/AnyExecutor.h"

#include "wondermake-utility/Typedefs.h"

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
