#include "pch.h"
#include "Engine.h"

#include "Graphics/Renderer.h"

#include "Message/DispatchRouter.h"

#include "Job/JobSystem.h"

#include "WonderMakeBase/CmdLineArgsSystem.h"
#include "WonderMakeBase/SystemGlobal.h"

#include "Program/ImguiWrapper.h"

#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

#include "WonderMakeEngine/LoggerFileSystem.h"

#include "WonderMakeIo/FileSystem.h"

#include "WonderMakeBase/Logger.h"
#include "WonderMakeBase/ScheduleSystem.h"

using namespace MemoryUnitLiterals;

namespace Engine
{
	void RouteMessages();

	void Start(Info&& aInfo, Callbacks&& aCallbacks)
	{
		Logger::Get().SetFilters(aInfo.Logging.AllowedSeverities, aInfo.Logging.Level);

		auto&& sysRegistry = Global::GetSystemRegistry();

		SystemContainer loggerContainer;

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STPlatformInterface, STLogger>::ToObject() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			bool logFileError = false;

			if (aInfo.Logging.File)
			{
				auto&& fileInfo = *aInfo.Logging.File;

				loggerContainer = std::move(result);

				auto&& fileLogger = loggerContainer.Get<LoggerFileSystem>();

				fileLogger.SetLogSizeLimits(fileInfo.TrimSize, fileInfo.MaxSize);

				logFileError = !fileLogger.OpenLogFile(aInfo.ProjectFolderNames / fileInfo.Path, fileInfo.Filename);
			}

			WM_LOG_INFO("");
			WM_LOG_INFO("");
			WM_LOG_INFO("");
			WM_LOG_INFO("---------------- WonderMake ----------------");
			WM_LOG_INFO("Started logging.");

			if (logFileError)
				WM_LOG_ERROR("Failed to open log file.");
		}

		{
			WM_LOG_INFO("Creating single instance systems...");

			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::ToObject<STSingleton>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WM_LOG_ERROR("Failed to create singleton systems; error: ", static_cast<SystemRegistry::ECreateError>(result), ", meta: ", result.Meta(), ".");

				return;
			}

			SystemContainer singletonContainer = result;

			auto&& fileSystem = singletonContainer.Get<FileSystem>();

			fileSystem.SetFolderSuffix(FolderLocation::Data, aInfo.ProjectFolderNames);
			fileSystem.SetFolderSuffix(FolderLocation::User, aInfo.ProjectFolderNames);
			fileSystem.SetFolderSuffix(FolderLocation::UserData, aInfo.ProjectFolderNames);

			WM_LOG_INFO("Single instance systems created.");
		}

		{
			WM_LOG_INFO("Registering core systems...");

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

			WM_LOG_INFO("Setting up filters...");

			SystemRegistry::Filter filter;

			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			WM_LOG_INFO("Creating systems...");

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WM_LOG_ERROR("Failed to create systems; error: ", static_cast<SystemRegistry::ECreateError>(result), ", meta: ", result.Meta(), ".");

				return;
			}

			sysContainer = std::move(result);

			auto&& timeKeeper = sysContainer.Get<TimeKeeper>();

			WM_LOG_SUCCESS("Engine is up and running.");

			std::move(aCallbacks.OnSetup)();

			WM_LOG_INFO("Starting main loop.");

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
