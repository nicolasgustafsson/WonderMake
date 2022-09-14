#pragma once

#include "wondermake-utility/Executor.h"

#include <vector>

class BufferExecutor
{
public:
	inline void Execute(Closure aClosure)
	{
		myClosures.emplace_back(std::move(aClosure));
	}
	inline void ExecuteAll()
	{
		for (auto& closure : myClosures)
			std::move(closure)();
	}

private:
	std::vector<Closure> myClosures;

};
