#pragma once

#include "wondermake-utility/UniqueFunction.h"

template<typename TExecutor>
concept CExecutor = requires(TExecutor aExecutor, Closure aClosure)
{
	aExecutor.Execute(std::move(aClosure));
};

class InlineExecutor
{
public:
	template<typename TCallable>
	inline void Execute(TCallable aClosure) const
	{
		std::move(aClosure)();
	}
};
