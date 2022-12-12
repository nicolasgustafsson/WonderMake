#pragma once

#include "wondermake-utility/Executor.h"

#include <functional>
#include <type_traits>

class AnyExecutor
{
public:
	AnyExecutor() noexcept = default;
	template<CExecutor TExecutor>
	inline AnyExecutor(TExecutor&& aExecutor) noexcept requires(!std::is_same_v<AnyExecutor, std::decay_t<TExecutor>>)
	{
		myExecute = [executor = std::forward<TExecutor>(aExecutor)](Closure&& aClosure)
		{
			executor.Execute(std::move(aClosure));
		};
	}
	AnyExecutor(const AnyExecutor&) noexcept = default;
	AnyExecutor(AnyExecutor&&) noexcept = default;

	AnyExecutor& operator=(const AnyExecutor&) noexcept = default;
	AnyExecutor& operator=(AnyExecutor&&) noexcept = default;

	void Execute(Closure&& aClosure) const;

private:
	std::function<void(Closure&&)> myExecute;

};
