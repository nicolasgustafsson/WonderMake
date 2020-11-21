#include "pch.h"
#include "DataThreads.h"

#include "Program/Program.h"

#include "Threads/Routine.h"

#include "Utilities/TimeKeeper.h"

void DataThreads::Start(Program& aProgramReference, Closure&& aCallback)
{
	myRoutine.AddProcedure([&aProgramReference]()
		{
			aProgramReference.Update();
		});

	SystemContainer::Get().CreateAllSystems();

	aCallback();

	for (;;)
	{
		//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
		SystemPtr<TimeKeeper>()->Update();

		myRoutine.Run();
	}
}

Routine& DataThreads::GetRoutine()
{
	return myRoutine;
}
