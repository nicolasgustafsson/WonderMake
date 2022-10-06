#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/Executor.h"

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

	void DelegateToFake()
	{
		ON_CALL(*this, Execute)
			.WillByDefault([](auto aClosure)
				{
					std::move(aClosure)();
				});
	}

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
