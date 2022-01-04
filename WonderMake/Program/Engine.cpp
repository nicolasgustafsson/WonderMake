#include "pch.h"
#include "Engine.h"

#include "FileSystem/FileSystem.h"

#include "Job/JobSystem.h"

#include "System/SystemGlobal.h"

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

		auto&& sysRegistry = Global::GetSystemRegistry();
		auto&& sysContainer = Global::GetSystemContainer();

		sysRegistry.AddSystem<JobSystem>([&sysContainer]() -> std::shared_ptr<JobSystem>
			{
				return std::make_shared<JobSystem>(sysContainer);
			});
		sysRegistry.AddSystem<ScheduleSystem>([&scheduleProc, &scheduleRepeatingProc]() -> std::shared_ptr<ScheduleSystem>
			{
				return std::make_shared<ScheduleSystem>(scheduleProc, scheduleRepeatingProc);
			});

		sysContainer = sysRegistry.CreateSystems();

		auto&& fileSystem = sysContainer.Get<FileSystem>();

		fileSystem.SetFolderSuffix(FolderLocation::Data, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::User, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::UserData, std::move(aProjectFolderNames));

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

