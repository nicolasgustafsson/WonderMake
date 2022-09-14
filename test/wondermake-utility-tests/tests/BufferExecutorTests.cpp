#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/BufferExecutor.h"

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

TEST(BufferExecutorTests, executor_does_not_invoke_callable_on_execute)
{
	StrictMock<ExecutorCallableMock> callableMock;
	BufferExecutor executor;

	executor.Execute(callableMock.GetCallable());
}

TEST(BufferExecutorTests, executor_invoke_callable_on_executeall)
{
	StrictMock<ExecutorCallableMock> callableMock;
	BufferExecutor executor;

	executor.Execute(callableMock.GetCallable());

	EXPECT_CALL(callableMock, Executed);

	executor.ExecuteAll();
}

TEST(BufferExecutorTests, executor_invoke_several_callables_on_executeall)
{
	StrictMock<ExecutorCallableMock> callableMock;
	BufferExecutor executor;

	executor.Execute(callableMock.GetCallable());
	executor.Execute(callableMock.GetCallable());
	executor.Execute(callableMock.GetCallable());

	EXPECT_CALL(callableMock, Executed)
		.Times(3);

	executor.ExecuteAll();
}
