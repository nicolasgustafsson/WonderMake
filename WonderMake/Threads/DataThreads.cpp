#include "stdafx.h"
#include "DataThreads.h"

#include "Threads/Routine.h"
#include "Threads/RoutineIds.h"
#include "Threads/RoutineMain.h"

void DataThreads::Start(Program& aProgramReference, Closure&& aCallback)
{
	myRoutines.resize(RoutineCount);
	myRoutines[static_cast<size_t>(ERoutineId::Logic)] = std::make_unique<RoutineMain>(aProgramReference);
	myRoutines[static_cast<size_t>(ERoutineId::Render)] = std::make_unique<Routine>(ERoutineId::Render);
	myRoutines[static_cast<size_t>(ERoutineId::File)] = std::make_unique<Routine>(ERoutineId::File);

	if constexpr(Constants::IsDebugging)
		myRoutines[static_cast<size_t>(ERoutineId::Debug)] = std::make_unique<Routine>(ERoutineId::Debug);
	
	myFileThread.emplace("File Thread").AddRoutine(*myRoutines[static_cast<size_t>(ERoutineId::File)]);
	myRenderThread.emplace("Render Thread").AddRoutine(*myRoutines[static_cast<size_t>(ERoutineId::Render)]);

	aCallback();

	for (;;)
	{
		myFileThread->DoOnce();
		myRenderThread->DoOnce();
		myRoutines[static_cast<size_t>(ERoutineId::Logic)]->Run();
		myRenderThread->WaitUntilReady();

		if constexpr (Constants::IsDebugging)
		{
			//wait for file thread so we can debug it as well.
			myFileThread->WaitUntilReady();
			myRoutines[static_cast<size_t>(ERoutineId::Debug)]->Run();
		}
	}
}

Routine& DataThreads::GetRoutine(const ERoutineId aRoutine)
{
	return *myRoutines[static_cast<size_t>(aRoutine)];
}
