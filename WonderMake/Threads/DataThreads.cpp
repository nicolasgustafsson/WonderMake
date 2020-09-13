#include "pch.h"
#include "DataThreads.h"

#include "Program/Program.h"

#include "Threads/Routine.h"
#include "Threads/RoutineIds.h"
#include "Threads/RoutineDebug.h"

#include "Utilities/Stopwatch.h"

DataThreads::DataThreads()
{
	myRoutines.resize(RoutineCount);
	myRoutines[static_cast<size_t>(ERoutineId::Logic)] = std::make_unique<Routine>(ERoutineId::Logic);
	myRoutines[static_cast<size_t>(ERoutineId::Render)] = std::make_unique<Routine>(ERoutineId::Render);
	myRoutines[static_cast<size_t>(ERoutineId::File)] = std::make_unique<Routine>(ERoutineId::File);

	if constexpr (Constants::IsDebugging)
		myRoutines[static_cast<size_t>(ERoutineId::Debug)] = std::make_unique<RoutineDebug>();
}

void DataThreads::Start(Program& aProgramReference, Closure&& aCallback)
{
	GetRoutine(ERoutineId::Logic).AddProcedure([&aProgramReference]()
		{
			aProgramReference.Update();
		});

	myFileThread.emplace("File Thread").AddRoutine(*myRoutines[static_cast<size_t>(ERoutineId::File)]);
	myRenderThread.emplace("Render Thread").AddRoutine(*myRoutines[static_cast<size_t>(ERoutineId::Render)]);

	SystemContainer::Get().CreateAllSystems();

	aCallback();

	for (;;)
	{
		//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
		myTimeKeeper->Update();

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
