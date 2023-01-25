#include "Engine.h"

#include "Audio/AudioManager.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Camera/Display.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Message/DispatchRouter.h"
#include "Message/Messages.h"
#include "Program/ImguiWrapper.h"
#include "Program/Window.h"
#include "Threads/TaskManager.h"
#include "Utilities/Debugging/Debugged.h"
#include "Utilities/TimeKeeper.h"

#include "wondermake-engine/ConfigurationEngine.h"
#include "wondermake-engine/DeserializeConfigurationJob.h"
#include "wondermake-engine/LoggerFileSystem.h"
#include "wondermake-engine/LoggerRemoteSystem.h"
#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/DebugSystem.h"

#include "wondermake-io/ConfigurationIo.h"
#include "wondermake-io/PlatformFilePaths.h"
#include "wondermake-io/ReadFileJob.h"
#include "wondermake-io/WriteFileJob.h"

#include "wondermake-base/CmdLineArgsSystem.h"
#include "wondermake-base/ConfigurationGlobal.h"
#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/DependencySystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/JobGlobal.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/ProcessSystem.h"
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

		auto& sysRegistryRef = Global::GetSystemRegistry();

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STPlatform, STWonderMake>::ToObject() };
			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			auto result = sysRegistryRef.CreateSystems(filter);

			if (!result)
				return;

			SystemContainer platformContainer = std::move(result).Unwrap();

			if (!SetupPlatformFilePaths(platformContainer, aInfo.ProjectFolderNames))
				return;
		}

		auto taskManager = std::make_shared<TaskManager>();
		auto& sysContainer = Global::GetSystemContainer();

		auto scheduleProc = [&taskManager](AnyExecutor aExecutor, Closure aTask) -> auto
		{
			return taskManager->Schedule(std::move(aExecutor), std::move(aTask));
		};
		auto scheduleRepeatingProc = [&taskManager](AnyExecutor aExecutor, std::function<void()> aTask) -> auto
		{
			return taskManager->ScheduleRepeating(std::move(aExecutor), std::move(aTask));
		};

		sysRegistryRef.AddSystem<JobSystem>([&sysContainer, &taskManager]() -> std::shared_ptr<JobSystem>
		{
			struct SScheduleExecutor
			{
				std::function<void(Closure)> Executor;

				void Execute(Closure&& aCallable) const
				{
					Executor(std::move(aCallable));
				}
			};

			static auto jobSys = std::make_shared<JobSystem>(JobGlobal::GetRegistry(), sysContainer, SScheduleExecutor
				{
					.Executor = [taskManager](Closure aCallable) { taskManager->Schedule(InlineExecutor(), std::move(aCallable)).Detach(); }
				});

			return jobSys;
		});
		sysRegistryRef.AddSystem<ConfigurationSystem>([&aInfo]()
			{
				const auto create = [&aInfo]()
				{
					auto config = std::make_shared<ConfigurationSystem>();
					const auto& filePathData = FilePathData::Get();

					std::optional<ConfigurationEngine::SGraphics>	graphics;
					std::optional<ConfigurationEngine::SAudio>		audio;

					if (!aInfo.Headless)
					{
						if (aInfo.Graphics)
							graphics.emplace(ConfigurationEngine::SGraphics
								{
									.WindowTitle			= (!aInfo.Graphics->WindowTitle.empty() ? aInfo.Graphics->WindowTitle : aInfo.ApplicationName),
									.MissingTexturePath		= aInfo.Graphics->MissingTexture,
									.DefaultRenderGraphPath	= aInfo.Graphics->DefaultRenderGraphPath,
									.ImguiIniLocation		= aInfo.Graphics->ImguiIniLocation,
									.ImguiFontDirectory		= aInfo.Graphics->ImguiFontDirectory
								});
						if (aInfo.Audio)
							audio.emplace(ConfigurationEngine::SAudio
								{
									aInfo.Audio->MainNodeGraph
								});
					}
					
					config->Initialize();

					ConfigurationIo::Configure(
						*config,
						filePathData.GetPathData(),
						filePathData.GetPathUser(),
						filePathData.GetPathUserData());
					ConfigurationEngine::Configure(
						*config,
						aInfo.Configuration.OverrideFileApplication.string(),
						aInfo.Configuration.OverrideFileDevice,
						aInfo.Configuration.OverrideFileUser,
						std::move(graphics),
						std::move(audio));

					GlobalConfiguration::GetRegistry().Configure(*config);

					return config;
				};

				static auto config = create();

				return config;
			});
		sysRegistryRef.AddSystem<CmdLineArgsSystem>([cmdLineArgs = aInfo.CommandLineArguments]()->std::shared_ptr<CmdLineArgsSystem>
		{
			static auto instance = std::make_shared<CmdLineArgsSystem>(cmdLineArgs);

			return instance;
		});
		sysRegistryRef.AddSystem<ScheduleSystemSingleton>([&scheduleProc, &scheduleRepeatingProc]() -> std::shared_ptr<ScheduleSystemSingleton>
			{
				static auto instance = std::make_shared<ScheduleSystemSingleton>(scheduleProc, scheduleRepeatingProc);

				return instance;
			});

		auto sysRegistry = sysRegistryRef;

		sysRegistry.AddSystem<ScheduleSystem>([&scheduleProc, &scheduleRepeatingProc]() -> std::shared_ptr<ScheduleSystem>
			{
				return std::make_shared<ScheduleSystem>(scheduleProc, scheduleRepeatingProc);
			});

		ProcessId currentProcessId;

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STFoundational, STWonderMake>::ToObject() };
			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			sysContainer = std::move(result).Unwrap();

			if (auto processSys = sysContainer.TryGet<ProcessSystem>())
				Logger::Get().SetProcessId(processSys->GetCurrentProcessId());

			currentProcessId = sysContainer.Get<ProcessSystem>().GetCurrentProcessId();

			JobSystem::InjectDependencies(std::tie());

			auto jobSystem = std::make_shared<JobSystem>(JobGlobal::GetRegistry(), sysContainer, InlineExecutor());
			auto& configurationSystem = sysContainer.Get<ConfigurationSystem>();

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
				.ThenApply(InlineExecutor(), [jobSystem, &configurationSystem, &aInfo](auto aFuture)
					{
						if (aFuture.IsCompleted())
						{
							auto resultOpt = std::move(aFuture).GetResult();

							if (resultOpt && !resultOpt.value())
								WmLogError(TagWonderMake << "Failed to deserialize application override file. Error: " << resultOpt->Err() << '.');
						}

						auto path = configurationSystem.Get<FilePath>(ConfigurationEngine::OverrideFileDevice, aInfo.Configuration.OverrideFileDevice);

						return jobSystem->StartJob<ReadFileJob>(std::move(path));
					})
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::Device, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenApply(InlineExecutor(), [jobSystem, &configurationSystem, &aInfo](auto aFuture)
					{
						if (aFuture.IsCompleted())
						{
							auto resultOpt = std::move(aFuture).GetResult();

							if (resultOpt && !resultOpt.value())
								WmLogError(TagWonderMake << "Failed to deserialize device override file. Error: " << resultOpt->Err() << '.');
						}

						auto path = configurationSystem.Get<FilePath>(ConfigurationEngine::OverrideFileUser, aInfo.Configuration.OverrideFileUser);

						return jobSystem->StartJob<ReadFileJob>(std::move(path));
					})
				.ThenApply(InlineExecutor(), FutureApplyResult([jobSystem](auto aResult)
					{
						if (!aResult)
							return MakeCanceledFuture<DeserializeConfigurationJob::Output>();

						std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

						return jobSystem->StartJob<DeserializeConfigurationJob>(EConfigGroup::User, std::string(jsonBlob.begin(), jsonBlob.end()));
					}))
				.ThenRun(InlineExecutor(), [](auto aFuture)
					{
						if (aFuture.IsCompleted())
						{
							auto resultOpt = std::move(aFuture).GetResult();

							if (resultOpt && !resultOpt.value())
								WmLogError(TagWonderMake << "Failed to deserialize user override file. Error: " << resultOpt->Err() << '.');
						}
					})
				.Detach();
		}

		std::shared_ptr<LoggerRemoteConnection> loggerRemoteConnection;
		std::shared_ptr<LoggerRemoteSocket> loggerRemoteSocket;

		{
			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STFoundational, STWonderMake>::ToObject(), STrait::Set<STLogger, STWonderMake>::ToObject() };
			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
				return;

			bool logFileError = false;
			sysContainer = std::move(result).Unwrap();

			if (aInfo.Logging.File)
			{
				auto&& logFileInfo = *aInfo.Logging.File;
				auto&& fileLogger = sysContainer.Get<LoggerFileSystem>();

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

				auto&& loggerRemote = sysContainer.Get<LoggerRemoteSystem>();

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

				auto&& loggerRemote = sysContainer.Get<LoggerRemoteSystem>();

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

		WmLogInfo(TagWonderMake << "Version: " << aInfo.Version << '.');

		{
			WmLogInfo(TagWonderMake << "Creating single instance systems...");

			SystemRegistry::Filter filter;

			filter.RequiredAnyTraits = { STrait::Set<STSingleton, STWonderMake>::ToObject() };
			if (aInfo.Headless)
				filter.DisallowedTraits = { STrait::ToObject<STGui>() };

			auto result = sysRegistry.CreateSystems(filter);

			if (!result)
			{
				WmLogError(TagWonderMake << "Failed to create singleton systems; error: " << result.Err() << '.');

				return;
			}

			sysContainer = std::move(result).Unwrap();
		}

		taskManager->SetImmediate(false);

		{
			WmLogInfo(TagWonderMake << "Registering core systems...");

			WmLogInfo(TagWonderMake << "Setting up filters...");

			SystemRegistry::Filter filter;

			if (!aInfo.RequiredSystemTraits.empty())
				filter.RequiredAnyTraits = { STrait::Set<STWonderMake>::ToObject(), aInfo.RequiredSystemTraits };
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

						const auto	group = *groupOpt;
						FilePath	path;

						switch (group)
						{
						case EConfigGroup::Application:	path = FilePath(FilePath::EFolder::Bin, configSys.Get<std::string>(ConfigurationEngine::OverrideFileApplication, aInfo.Configuration.OverrideFileApplication.string()));	break;
						case EConfigGroup::Device:		path = configSys.Get<FilePath>(ConfigurationEngine::OverrideFileDevice, aInfo.Configuration.OverrideFileDevice);															break;
						case EConfigGroup::User:		path = configSys.Get<FilePath>(ConfigurationEngine::OverrideFileUser, aInfo.Configuration.OverrideFileUser);																break;
						}
						
						jobSys
							.StartJob<SerializeConfigurationJob>(group)
							.ThenApply(InlineExecutor(), FutureApplyResult([&jobSys, path = std::move(path)](auto aJsonBlob) mutable
								{
									return jobSys.StartJob<WriteFileJob>(std::move(path), std::move(aJsonBlob));
								}))
							.Detach();
					})
					.Detach();
			}

			{
				auto* dependencySys = sysContainer.TryGet<DependencySystem>();

				if (dependencySys)
					dependencySys->SetSystems(sysContainer);
				else
					WmLogError(TagWonderMake << "Failed to get DependencySystem.");
			}

			auto& timeKeeper = sysContainer.Get<TimeKeeperSingleton>();
			
			WmLogSuccess(TagWonderMake << "Engine is up and running.");

			std::move(aCallbacks.OnSetup)();

			WmLogInfo(TagWonderMake << "Starting main loop.");

			bool isF3DownLastFrame = false;

			for (;;)
			{
				//update the timekeeper before any threads have run so that delta time can be accessed asynchronously
				timeKeeper.Update();

				if (!aInfo.Headless)
				{
					auto& audioManager			= sysContainer.Get<AudioManager>();
					auto& inputSys				= sysContainer.Get<InputSystem>();
					auto& engineUniformBuffer	= sysContainer.Get<EngineUniformBuffer>();
					auto& window				= sysContainer.Get<Window>();
					auto& renderer				= sysContainer.Get<Renderer>();
					auto& imguiWrapper			= sysContainer.Get<ImguiWrapper>();

					audioManager.Update();
					inputSys.Update();

					engineUniformBuffer.GetBuffer().Time = timeKeeper.GetTotalTime<WmChrono::fSeconds>().count();

					engineUniformBuffer.Update();

					window.Update();

					renderer.StartFrame();

					imguiWrapper.StartFrame();
				}

				taskManager->Update();

				RouteMessages();

				if (!aInfo.Headless)
				{
					auto& debugSys				= sysContainer.Get<DebugSystem>();
					auto& renderer				= sysContainer.Get<Renderer>();
					auto& inputSys				= sysContainer.Get<InputSystem>();
					auto& router				= DispatchRouter::Get();
					auto& imguiWrapper			= sysContainer.Get<ImguiWrapper>();

					auto newDebugged = Debugged::GetAndResetDebugged();

					for (auto& [name, tick] : newDebugged)
						debugSys.AddDebugWindow(std::move(name), std::move(tick));

					renderer.FinishFrame();

					 // TODO: Move this into DebugSystem.
					const bool isF3Down = inputSys.GetKeyState(EKeyboardKey::F3, InputSystem::EFocus::Window) == EInputItemState::Released;

					if (!isF3DownLastFrame && isF3Down)
						debugSys.ToggleToolbar();

					isF3DownLastFrame = isF3Down;

					router.RouteDispatchable(SDebugMessage());

					router.CommitChanges();

					debugSys.Tick();
					
					imguiWrapper.EndFrame();

					renderer.SwapBuffers();
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
