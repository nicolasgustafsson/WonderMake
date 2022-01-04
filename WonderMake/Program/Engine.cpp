#include "pch.h"
#include "Engine.h"

#include "FileSystem/FileSystem.h"

#include "Graphics/Renderer.h"

#include "Message/DispatchRouter.h"

#include "Job/JobSystem.h"

#include "System/SystemGlobal.h"

#include "Program/ImguiWrapper.h"

#include "Scheduling/ScheduleSystem.h"

#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

namespace Engine
{
	void RouteMessages();

	void Start(std::filesystem::path&& aProjectFolderNames, Closure&& aCallback)
	{
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
		auto&& timeKeeper = sysContainer.Get<TimeKeeper>();

		fileSystem.SetFolderSuffix(FolderLocation::Data, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::User, aProjectFolderNames);
		fileSystem.SetFolderSuffix(FolderLocation::UserData, std::move(aProjectFolderNames));

		aCallback();

		for (;;)
		{
			//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
			timeKeeper.Update();

			taskManager.Update();
			
			RouteMessages();

			if constexpr (Constants::IsDebugging)
			{
				auto&& renderer = sysContainer.Get<Renderer>();
				auto&& imguiWrapper = sysContainer.Get<ImguiWrapper>();

				renderer.FinishFrame();
				imguiWrapper.StartFrame();

				auto&& router = DispatchRouter::Get();

				router.RouteDispatchable(SDebugMessage());

				router.CommitChanges();

				imguiWrapper.EndFrame();
			}
		}
	}

	void RouteMessages()
	{
		std::vector<std::shared_ptr<Dispatchable>> messageList;

		auto&& router = DispatchRouter::Get();
		std::swap(DispatchableBuffer::Get().myList, messageList);

		for (const auto& message : messageList)
		{
			router.RouteDispatchable(*message);

			router.CommitChanges();
		}
	}
}
