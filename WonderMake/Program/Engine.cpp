#include "pch.h"
#include "Engine.h"

#include "Job/JobSystem.h"

#include "Program/Program.h"

#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include "Threads/Routine.h"
#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

namespace Engine
{
	void Start(const Closure& aCallback)
	{
		Program program;
		Routine routine;
		TaskManager taskManager;

		taskManager.ScheduleRepeating([&program]()
			{
				program.Update();
			});

		auto scheduleProc = [&taskManager](Closure aTask)
		{
			taskManager.Schedule(std::move(aTask));
		};
		auto scheduleRepeatingProc = [&taskManager](Closure aTask)
		{
			taskManager.ScheduleRepeating(std::move(aTask));
		};

		SystemContainer::Get().AddSystem<JobSystem>([]() -> JobSystem&
			{
				static JobSystem system(SystemContainer::Get());

				return system;
			});
		SystemContainer::Get().AddSystem<ScheduleSystem>([&scheduleProc, &scheduleRepeatingProc]() -> ScheduleSystem&
			{
				static ScheduleSystem system(scheduleProc, scheduleRepeatingProc);

				return system;
			});

		SystemContainer::Get().CreateAllSystems();

		aCallback();

		for (;;)
		{
			//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
			SystemPtr<TimeKeeper>()->Update();

			taskManager.Update();
			routine.Run();
		}
	}
}

