#include "pch.h"
#include "Engine.h"

#include "Graphics/Renderer.h"

#include "Message/DispatchRouter.h"

#include "Job/JobSystem.h"

#include "WonderMakeEngine/CmdLineArgsSystem.h"
#include "WonderMakeEngine/SystemGlobal.h"

#include "Program/ImguiWrapper.h"

#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

#include "WonderMakeEngine/Logger.h"
#include "WonderMakeEngine/ScheduleSystem.h"

#include "WonderMakeIo/FileSystem.h"

namespace Engine
{
	void RouteMessages();

	void Start(Info&& aInfo, Callbacks&& aCallbacks)
	{
		Logger::Get().SetFilters(
			{ ELogSeverity::Success, ELogSeverity::Info, ELogSeverity::Warning, ELogSeverity::Error },
			{ ELogLevel::Debug, ELogLevel::Verbose, ELogLevel::Normal, ELogLevel::Priority });

		auto&& sysRegistry = Global::GetSystemRegistry();

		SystemContainer loggerContainer;

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STPlatformInterface, STLogger>::ToObject() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			loggerContainer = std::move(result);
		}

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::ToObject<STSingleton>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WM_LOG_ERROR("Failed to create singleton systems; error: " << static_cast<SystemRegistry::ECreateError>(result) << ", meta: " << result.Meta() << ".");

				return;
			}

			SystemContainer singletonContainer = result;

			auto&& fileSystem = singletonContainer.Get<FileSystem>();

			fileSystem.SetFolderSuffix(FolderLocation::Data, aInfo.ProjectFolderNames);
			fileSystem.SetFolderSuffix(FolderLocation::User, aInfo.ProjectFolderNames);
			fileSystem.SetFolderSuffix(FolderLocation::UserData, aInfo.ProjectFolderNames);
		}

		{
			auto&& sysContainer = Global::GetSystemContainer();

			TaskManager taskManager;

			auto scheduleProc = [&taskManager](Closure aTask)
			{
				taskManager.Schedule(std::move(aTask));
			};
			auto scheduleRepeatingProc = [&taskManager](std::function<void()> aTask)
			{
				taskManager.ScheduleRepeating(std::move(aTask));
			};

			sysRegistry.AddSystem<CmdLineArgsSystem>([&cmdLineArgs = aInfo.CommandLineArguments]() -> std::shared_ptr<CmdLineArgsSystem>
			{
				return std::make_shared<CmdLineArgsSystem>(cmdLineArgs);
			});
			sysRegistry.AddSystem<JobSystem>([&sysContainer]() -> std::shared_ptr<JobSystem>
				{
					return std::make_shared<JobSystem>(sysContainer);
				});
			sysRegistry.AddSystem<ScheduleSystem>([&scheduleProc, &scheduleRepeatingProc]() -> std::shared_ptr<ScheduleSystem>
				{
					return std::make_shared<ScheduleSystem>(scheduleProc, scheduleRepeatingProc);
				});

			SystemRegistry::Filter filter;

			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WM_LOG_ERROR("Failed to create systems; error: " << static_cast<SystemRegistry::ECreateError>(result) << ", meta: " << result.Meta() << ".");

				return;
			}

			sysContainer = std::move(result);

			auto&& timeKeeper = sysContainer.Get<TimeKeeper>();

			std::move(aCallbacks.OnSetup)();

			for (;;)
			{
				//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
				timeKeeper.Update();

				taskManager.Update();

				RouteMessages();

				if constexpr (Constants::IsDebugging)
					if (!aInfo.Headless)
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
	}

	void RouteMessages()
	{
		std::vector<std::shared_ptr<Dispatchable>> messageList;

		auto&& router = DispatchRouter::Get();
		std::swap(DispatchableBuffer::Get().myList, messageList);

		for (const auto& message : messageList)
		{
			router.CommitChanges();

			router.RouteDispatchable(*message);
		}
	}
}
