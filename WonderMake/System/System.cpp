#include "stdafx.h"
#include "System.h"

#include "Threads/DataThreads.h"
#include "Threads/Routine.h"

void System::EnableTick()
{
	DataThreads::Get().GetRoutine(ERoutineId::Logic)->AddProcedure([this] {Tick(); });
}
