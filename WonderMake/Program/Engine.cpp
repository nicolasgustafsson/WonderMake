#include "stdafx.h"
#include "Engine.h"
#include "Threads/DataThreads.h"
#include "Program/Program.h"

namespace Engine
{
	void Start()
	{
		Program program;
		DataThreads::Get().Start(program);
	}
}

