#include "pch.h"
#include "Engine.h"

#include "Graphics/Renderer.h"

#include "Message/DispatchRouter.h"

#include "Program/ImguiWrapper.h"

#include "Threads/TaskManager.h"

#include "Utilities/TimeKeeper.h"

#include "wondermake-engine/ConfigurationEngine.h"
#include "wondermake-engine/DeserializeConfigurationJob.h"
#include "wondermake-engine/LoggerFileSystem.h"
#include "wondermake-engine/LoggerRemoteSystem.h"

#include "wondermake-io/ConfigurationIo.h"
#include "wondermake-io/FileSystem.h"
#include "wondermake-io/ReadFileJob.h"

#include "wondermake-base/CmdLineArgsSystem.h"
#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/JobGlobal.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/SystemGlobal.h"

using namespace MemoryUnitLiterals;

namespace Engine
{
	void RouteMessages();

	void Start(Info&& aInfo, Callbacks&& aCallbacks)
	{
		Logger::Get().SetLoggerName(aInfo.Logging.LoggerName);
		Logger::Get().SetFilters(aInfo.Logging.AllowedSeverities, aInfo.Logging.Level);

		auto&& sysRegistry = Global::GetSystemRegistry();

		sysRegistry.AddSystem<ConfigurationSystem>([configurationInfo = aInfo.Configuration]()
			{
				const auto create = [&configurationInfo]()
				{
					auto config = std::make_shared<ConfigurationSystem>();

					config->Initialize();

					ConfigurationIo::Configure(*config);
					ConfigurationEngine::Configure(
						*config,
						configurationInfo.OverrideFileApplication.string(),
						configurationInfo.OverrideFileDevice.string(),
						configurationInfo.OverrideFileUser.string(),
						configurationInfo.OverrideFileUserLocation == EOverrideFileUserLocation::User ? ConfigurationEngine::EOverrideFileUserLocation::User : ConfigurationEngine::EOverrideFileUserLocation::UserData);

					return config;
				};

				static auto config = create();

				return config;
			});

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STFoundational>::ToObject() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			SystemContainer foundationalContainer = std::move(result);

			auto&& fileSystem = foundationalContainer.Get<FileSystem>();
 
 			fileSystem.SetFolderSuffix(FolderLocation::Data,		aInfo.ProjectFolderNames);
 			fileSystem.SetFolderSuffix(FolderLocation::User,		aInfo.ProjectFolderNames);
 			fileSystem.SetFolderSuffix(FolderLocation::UserData,	aInfo.ProjectFolderNames);

			JobSystem::InjectDependencies(std::tie());

			auto jobSystem = std::make_shared<JobSystem>(JobGlobal::GetRegistry(), foundationalContainer, InlineExecutor());
			auto&& configurationSystem = foundationalContainer.Get<ConfigurationSystem>();

			jobSystem->StartJob<ReadFileJob>(FolderLocation::Bin, configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileApplication, aInfo.Configuration.OverrideFileApplication.string()))
				.ThenApply(InlineExecutor(), MoveFutureResult([jobSystem, &configurationSystem](auto aResult)
					{
						if (!aResult)
							return Future<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult);

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::Application, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenApply(InlineExecutor(), MoveFutureResult([jobSystem, &configurationSystem, &aInfo](auto aResult)
					{
						if (!aResult)
							return Future<ReadFileJob::Output>();

						auto path = configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileDevice, aInfo.Configuration.OverrideFileDevice.string());

						return jobSystem->StartJob<ReadFileJob>(FolderLocation::Data, std::move(path));
					}))
				.ThenApply(InlineExecutor(), MoveFutureResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return Future<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult);

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::Device, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenApply(InlineExecutor(), MoveFutureResult([jobSystem, &configurationSystem, &aInfo](auto aResult)
					{
						if (!aResult)
							return Future<ReadFileJob::Output>();

						const auto userLocation = configurationSystem.Get<ConfigurationEngine::EOverrideFileUserLocation>(ConfigurationEngine::OverrideFileUserLocation, ConfigurationEngine::EOverrideFileUserLocation::UserData);

						const auto folderLocation = userLocation == ConfigurationEngine::EOverrideFileUserLocation::User ? FolderLocation::User : FolderLocation::UserData;
						auto path = configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileUser, aInfo.Configuration.OverrideFileUser.string());

						return jobSystem->StartJob<ReadFileJob>(folderLocation, std::move(path));
					}))
				.ThenApply(InlineExecutor(), MoveFutureResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return Future<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult);

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::User, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.Detach();
		}

		SystemContainer loggerContainer;
		std::shared_ptr<LoggerRemoteConnection> loggerRemoteConnection;
		std::shared_ptr<LoggerRemoteSocket> loggerRemoteSocket;

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STFoundational, STLogger>::ToObject() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			bool logFileError = false;

			loggerContainer = std::move(result);

			if (aInfo.Logging.File)
			{
				auto&& logFileInfo = *aInfo.Logging.File;
				auto&& fileLogger = loggerContainer.Get<LoggerFileSystem>();

				fileLogger.SetLogSizeLimits(logFileInfo.TrimSize, logFileInfo.MaxSize);

				logFileError = !fileLogger.OpenLogFile(logFileInfo.Path, logFileInfo.Filename);
			}

			WM_LOG_INFO("");
			WM_LOG_INFO("");
			WM_LOG_INFO("");
			WM_LOG_INFO("---------------- ", aInfo.ApplicationName, " ----------------");
			WM_LOG_SUCCESS("Started logging.");

			if (aInfo.Logging.IpcConnection)
			{
				auto&& connectionInfo = *aInfo.Logging.IpcConnection;

				WM_LOG_INFO("Opening IPC logging connection, name: ", connectionInfo.Name, ".");

				auto&& loggerRemote = loggerContainer.Get<LoggerRemoteSystem>();

				auto connectionResult = loggerRemote.ConnectIpc(connectionInfo.Name);

				if (connectionResult)
				{
					WM_LOG_INFO("IPC log connection opened, name: ", connectionInfo.Name, ".");
					loggerRemoteConnection = connectionResult;
				}
				else
					WM_LOG_ERROR("Failed to open IPC log connection, error: ", static_cast<IpcConnection::ConnectionError>(connectionResult), ".");
			}
			if (aInfo.Logging.IpcSocket)
			{
				auto&& socketInfo = *aInfo.Logging.IpcSocket;

				WM_LOG_INFO("Opening IPC logging socket, name: ", socketInfo.Name, ".");

				auto&& loggerRemote = loggerContainer.Get<LoggerRemoteSystem>();

				auto socketResult = loggerRemote.OpenSocketIpc(socketInfo.Name);

				if (socketResult)
				{
					WM_LOG_INFO("IPC log socket opened, name: ", socketInfo.Name, ".");
					loggerRemoteSocket = socketResult;
				}
				else
					WM_LOG_ERROR("Failed to open IPC log socket, error: ", static_cast<IpcAcceptor::EOpenError>(socketResult), ".");
			}

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
		}

		{
			WM_LOG_INFO("Registering core systems...");

			auto&& sysContainer = Global::GetSystemContainer();

			auto taskManager = std::make_shared<TaskManager>();

			auto scheduleProc = [&taskManager](Closure aTask)
			{
				taskManager->Schedule(std::move(aTask));
			};
			auto scheduleRepeatingProc = [&taskManager](std::function<void()> aTask)
			{
				taskManager->ScheduleRepeating(std::move(aTask));
			};

			sysRegistry.AddSystem<CmdLineArgsSystem>([cmdLineArgs = aInfo.CommandLineArguments]() -> std::shared_ptr<CmdLineArgsSystem>
			{
				return std::make_shared<CmdLineArgsSystem>(cmdLineArgs);
			});
			sysRegistry.AddSystem<JobSystem>([&sysContainer, executor = taskManager->GetExecutor()]() -> std::shared_ptr<JobSystem>
				{
					return std::make_shared<JobSystem>(JobGlobal::GetRegistry(), sysContainer, executor);
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

				taskManager->Update();

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
