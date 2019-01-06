#include "stdafx.h"
#include "DataThreads.h"

#include "Threads/Routine.h"
#include "Threads/RoutineIds.h"
#include "Threads/RoutineMain.h"

void StartRoutine(std::shared_ptr<Routine> aRoutine)
{
	for(;;)
		aRoutine->Run();
}

void DataThreads::Start(Program& aProgramReference, Closure&& aCallback)
{
	myRoutines.resize(3);
	myRoutines[static_cast<size_t>(ERoutineId::Logic)] = std::make_shared<RoutineMain>(aProgramReference);
	myRoutines[static_cast<size_t>(ERoutineId::Render)] = std::make_shared<Routine>(ERoutineId::Render);
	myRoutines[static_cast<size_t>(ERoutineId::File)] = std::make_shared<Routine>(ERoutineId::File);
	
	myThreads.reserve(myRoutines.size() - 1);
	for (size_t i = 1; i < myThreads.size(); ++i)
	{
		myThreads.emplace_back(&StartRoutine, myRoutines[i]);
	}
	
	aCallback();

	StartRoutine(myRoutines[static_cast<size_t>(ERoutineId::Logic)]);
}

std::weak_ptr<Routine> DataThreads::GetRoutine(const ERoutineId aRoutine)
{
	return myRoutines[static_cast<size_t>(aRoutine)];
}
