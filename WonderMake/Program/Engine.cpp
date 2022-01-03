#include "pch.h"
#include "Engine.h"

#include "FileSystem/FileSystem.h"

#include "Job/JobSystem.h"

#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include "Threads/Routine.h"
#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

namespace Engine
{
	void Start(std::filesystem::path&& aProjectFolderNames, Closure&& aCallback)
	{
		Routine routine;
		TaskManager taskManager;

		auto scheduleProc = [&taskManager](Closure aTask)
		{
			taskManager.Schedule(std::move(aTask));
		};
		auto scheduleRepeatingProc = [&taskManager](Closure aTask)
		{
			taskManager.ScheduleRepeating(std::move(aTask));
		};

		auto&& sysContainer = SystemContainer::Get();

		sysContainer.AddSystem<JobSystem>([&sysContainer]() -> JobSystem&
			{
				static JobSystem system(sysContainer);

				return system;
			});
		sysContainer.AddSystem<ScheduleSystem>([&scheduleProc, &scheduleRepeatingProc]() -> ScheduleSystem&
			{
				static ScheduleSystem system(scheduleProc, scheduleRepeatingProc);

				return system;
			});

		auto&& fileSystem = sysContainer.GetSystem<FileSystem>();

		fileSystem.SetFolderSuffix(FolderLocation::Data, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::User, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::UserData, std::move(aProjectFolderNames));

		sysContainer.CreateAllSystems();

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

