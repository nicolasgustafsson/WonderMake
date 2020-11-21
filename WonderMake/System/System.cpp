#include "pch.h"
#include "System.h"

#include "Threads/DataThreads.h"
#include "Threads/Routine.h"

void SystemBase::EnableTick()
{
	DataThreads::Get().GetRoutine().AddProcedure([this] { Tick(); });
}
