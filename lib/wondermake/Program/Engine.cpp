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
#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"

#include "wondermake-io/ConfigurationIo.h"
#include "wondermake-io/FileSystem.h"
#include "wondermake-io/PlatformFilePaths.h"
#include "wondermake-io/ReadFileJob.h"
#include "wondermake-io/WriteFileJob.h"

#include "wondermake-base/CmdLineArgsSystem.h"
#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/JobGlobal.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/FilePathData.h"

using namespace MemoryUnitLiterals;

namespace Engine
{
	bool SetupPlatformFilePaths(const SystemContainer& aContainer, const std::filesystem::path& aProjectFolderNames);

	void RouteMessages();

	void Start(Info&& aInfo, Callbacks&& aCallbacks)
	{
		Logger::Get().SetLoggerName(aInfo.Logging.LoggerName);
		Logger::Get().SetFilters(aInfo.Logging.AllowedSeverities, aInfo.Logging.Level);

		auto&& sysRegistry = Global::GetSystemRegistry();

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STPlatform>::ToObject() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			SystemContainer platformContainer = std::move(result).Unwrap();

			if (!SetupPlatformFilePaths(platformContainer, aInfo.ProjectFolderNames))
				return;

		}
		
		sysRegistry.AddSystem<ConfigurationSystem>([&aInfo]()
			{
				const auto create = [&aInfo]()
				{
					auto config = std::make_shared<ConfigurationSystem>();
					const auto& filePathData = FilePathData::Get();

					config->Initialize();

					ConfigurationIo::Configure(
						*config,
						filePathData.GetPathData(),
						filePathData.GetPathUser(),
						filePathData.GetPathUserData());
					ConfigurationEngine::Configure(
						*config,
						aInfo.Configuration.OverrideFileApplication.string(),
						aInfo.Configuration.OverrideFileDevice.string(),
						aInfo.Configuration.OverrideFileUser.string(),
						aInfo.Configuration.OverrideFileUserLocation == EOverrideFileUserLocation::User ? ConfigurationEngine::EOverrideFileUserLocation::User : ConfigurationEngine::EOverrideFileUserLocation::UserData,
						aInfo.Headless);

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

			SystemContainer foundationalContainer = std::move(result).Unwrap();

			auto&& fileSystem = foundationalContainer.Get<FileSystem>();
 
 			fileSystem.SetFolderSuffix(FolderLocation::Data,		aInfo.ProjectFolderNames);
 			fileSystem.SetFolderSuffix(FolderLocation::User,		aInfo.ProjectFolderNames);
 			fileSystem.SetFolderSuffix(FolderLocation::UserData,	aInfo.ProjectFolderNames);

			JobSystem::InjectDependencies(std::tie());

			auto jobSystem = std::make_shared<JobSystem>(JobGlobal::GetRegistry(), foundationalContainer, InlineExecutor());
			auto&& configurationSystem = foundationalContainer.Get<ConfigurationSystem>();

			configurationSystem.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryData, InlineExecutor(), [](const auto& /*aId*/, const auto& aPath)
				{
					FilePathData::Get().SetPathData(aPath);
				})
				.Detach();
			configurationSystem.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryUser, InlineExecutor(), [](const auto& /*aId*/, const auto& aPath)
				{
					FilePathData::Get().SetPathUser(aPath);
				})
				.Detach();
			configurationSystem.OnOverrideChanged<std::string>(ConfigurationIo::ConfigDirectoryUserData, InlineExecutor(), [](const auto& /*aId*/, const auto& aPath)
				{
					FilePathData::Get().SetPathUserData(aPath);
				})
				.Detach();

			jobSystem->StartJob<ReadFileJob>(FilePath(FilePath::EFolder::Bin, configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileApplication, aInfo.Configuration.OverrideFileApplication.string())))
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem, &configurationSystem](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::Application, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem, &configurationSystem, &aInfo](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<ReadFileJob::Output>();

						auto path = configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileDevice, aInfo.Configuration.OverrideFileDevice.string());

						return jobSystem->StartJob<ReadFileJob>(FilePath(FilePath::EFolder::Data, std::move(path)));
					}))
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::Device, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem, &configurationSystem, &aInfo](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<ReadFileJob::Output>();

						const auto userLocation = configurationSystem.Get<ConfigurationEngine::EOverrideFileUserLocation>(ConfigurationEngine::OverrideFileUserLocation, ConfigurationEngine::EOverrideFileUserLocation::UserData);

						const auto folderLocation = userLocation == ConfigurationEngine::EOverrideFileUserLocation::User ? FilePath::EFolder::User : FilePath::EFolder::UserData;
						auto path = configurationSystem.Get<std::string>(ConfigurationEngine::OverrideFileUser, aInfo.Configuration.OverrideFileUser.string());

						return jobSystem->StartJob<ReadFileJob>(FilePath(folderLocation, std::move(path)));
					}))
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

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

			loggerContainer = std::move(result).Unwrap();

			if (aInfo.Logging.File)
			{
				auto&& logFileInfo = *aInfo.Logging.File;
				auto&& fileLogger = loggerContainer.Get<LoggerFileSystem>();

				fileLogger.SetLogSizeLimits(logFileInfo.TrimSize, logFileInfo.MaxSize);

				logFileError = !fileLogger.OpenLogFile(logFileInfo.Path);
			}

			WmLogInfo(TagWonderMake << "");
			WmLogInfo(TagWonderMake << "");
			WmLogInfo(TagWonderMake << "");
			WmLogInfo(TagWonderMake << "---------------- " << aInfo.ApplicationName << " ----------------");
			WmLogSuccess(TagWonderMake << "Started logging.");

			if (aInfo.Logging.IpcConnection)
			{
				auto&& connectionInfo = *aInfo.Logging.IpcConnection;

				WmLogInfo(TagWonderMake << "Opening IPC logging connection, name: " << connectionInfo.Name << '.');

				auto&& loggerRemote = loggerContainer.Get<LoggerRemoteSystem>();

				auto connectionResult = loggerRemote.ConnectIpc(connectionInfo.Name);

				if (connectionResult)
				{
					WmLogInfo(TagWonderMake << "IPC log connection opened, name: " << connectionInfo.Name << '.');
					loggerRemoteConnection = std::move(connectionResult).Unwrap();
				}
				else
					WmLogError(TagWonderMake << "Failed to open IPC log connection, error: " << connectionResult.Err() << '.');
			}
			if (aInfo.Logging.IpcSocket)
			{
				auto&& socketInfo = *aInfo.Logging.IpcSocket;

				WmLogInfo(TagWonderMake << "Opening IPC logging socket, name: " << socketInfo.Name << '.');

				auto&& loggerRemote = loggerContainer.Get<LoggerRemoteSystem>();

				auto socketResult = loggerRemote.OpenSocketIpc(socketInfo.Name);

				if (socketResult)
				{
					WmLogInfo(TagWonderMake << "IPC log socket opened, name: " << socketInfo.Name << '.');
					loggerRemoteSocket = std::move(socketResult).Unwrap();
				}
				else
					WmLogError(TagWonderMake << "Failed to open IPC log socket, error: " << socketResult.Err() << '.');
			}

			if (logFileError)
				WmLogError(TagWonderMake << "Failed to open log file.");
		}

		{
			WmLogInfo(TagWonderMake << "Creating single instance systems...");

			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::ToObject<STSingleton>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WmLogError(TagWonderMake << "Failed to create singleton systems; error: " << result.Err() << '.');

				return;
			}
		}

		{
			WmLogInfo(TagWonderMake << "Registering core systems...");

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

			WmLogInfo(TagWonderMake << "Setting up filters...");

			SystemRegistry::Filter filter;

			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			WmLogInfo(TagWonderMake << "Creating systems...");

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WmLogError(TagWonderMake << "Failed to create systems; error: " << result.Err() << '.');

				return;
			}

			sysContainer = std::move(result).Unwrap();
			
			{
				auto& configSys = sysContainer.Get<ConfigurationSystem>();
				auto& jobSys = sysContainer.Get<JobSystem>();

				configSys.OnOverrideChanged(InlineExecutor(), [&jobSys, &configSys, &aInfo](const auto& aId)
					{
						const auto groupOpt = configSys.GetGroup(aId);

						if (!groupOpt)
							return;

						const auto				group = *groupOpt;
						std::filesystem::path	path;
						FilePath::EFolder		folder = FilePath::EFolder::Bin;

						switch (group)
						{
						case EConfigGroup::Application:	path = configSys.Get<std::string>(ConfigurationEngine::OverrideFileApplication, aInfo.Configuration.OverrideFileApplication.string());	break;
						case EConfigGroup::Device:		path = configSys.Get<std::string>(ConfigurationEngine::OverrideFileDevice, aInfo.Configuration.OverrideFileDevice.string());			break;
						case EConfigGroup::User:		path = configSys.Get<std::string>(ConfigurationEngine::OverrideFileUser, aInfo.Configuration.OverrideFileUser.string());				break;
						}
						switch (group)
						{
						case EConfigGroup::Application:	folder = FilePath::EFolder::Bin; break;
						case EConfigGroup::Device:		folder = FilePath::EFolder::Data; break;
						case EConfigGroup::User:
						{
							const auto userLocation = configSys.Get<ConfigurationEngine::EOverrideFileUserLocation>(ConfigurationEngine::OverrideFileUserLocation, ConfigurationEngine::EOverrideFileUserLocation::UserData);

							folder = userLocation == ConfigurationEngine::EOverrideFileUserLocation::User ? FilePath::EFolder::User : FilePath::EFolder::UserData;

							break;
						}
						}

						jobSys
							.StartJob<SerializeConfigurationJob>(group)
							.ThenApply(InlineExecutor(), FutureApplyResult([&jobSys, folder, path = std::move(path)](auto aJsonBlob) mutable
								{
									return jobSys.StartJob<WriteFileJob>(FilePath(folder, std::move(path)), std::move(aJsonBlob));
								}))
							.Detach();
					})
					.Detach();
			}

			auto&& timeKeeper = sysContainer.Get<TimeKeeper>();

			WmLogSuccess(TagWonderMake << "Engine is up and running.");

			std::move(aCallbacks.OnSetup)();

			WmLogInfo(TagWonderMake << "Starting main loop.");

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
						auto&& debugSettings = sysContainer.Get<DebugSettingsSystem>();

						renderer.FinishFrame();
						imguiWrapper.StartFrame();

						debugSettings.TickAllWindows();

						auto&& router = DispatchRouter::Get();

						router.RouteDispatchable(SDebugMessage());

						router.CommitChanges();

						imguiWrapper.EndFrame();
					}
			}
		}
	}

	bool SetupPlatformFilePaths(const SystemContainer& aContainer, const std::filesystem::path& aProjectFolderNames)
	{
		auto pathBin		= PlatformFilePaths::GetFolderLocation(aContainer, FolderLocation::Bin);
		auto pathData		= PlatformFilePaths::GetFolderLocation(aContainer, FolderLocation::Data);
		auto pathUser		= PlatformFilePaths::GetFolderLocation(aContainer, FolderLocation::User);
		auto pathUserData	= PlatformFilePaths::GetFolderLocation(aContainer, FolderLocation::UserData);

		if (!pathBin || !pathData || !pathUser || !pathUserData)
			return false;

		FilePathData::Get().Initialize(
			std::move(*pathBin),
			std::move(*pathData)		/ aProjectFolderNames,
			std::move(*pathUser)		/ aProjectFolderNames,
			std::move(*pathUserData)	/ aProjectFolderNames);

		return true;
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
