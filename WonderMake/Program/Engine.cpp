#include "pch.h"
#include "Engine.h"
#include "Threads/DataThreads.h"
#include "Program/Program.h"
#include "System/SystemContainer.h"

namespace Engine
{
	void Start(Closure&& aCallback)
	{
		Program program;

		DataThreads::Get().Start(program, std::forward<Closure>(aCallback));
	}
}

