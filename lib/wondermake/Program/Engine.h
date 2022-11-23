#pragma once

#include "wondermake-base/LoggerTypes.h"

#include "wondermake-utility/CmdLineArgs.h"
#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/UniqueFunction.h"
#include "wondermake-utility/Typedefs.h"

#include <filesystem>
#include <optional>
#include <vector>
#include <unordered_set>

namespace Engine
{
	struct LogFileInfo
	{
		// The path to the log file.
		FilePath Path = FilePath(FilePath::EFolder::UserData, "log.txt");

		// Size the log file will be trimmed to when launching the engine or when exceeding the max size.
		MemoryUnit<EMemoryRatio::Bytes, uintmax_t> TrimSize = MemoryUnit<EMemoryRatio::MiB, uintmax_t>(1);

		// The maximum allowed size of the log file. If the file exceeds this amount it will be trimmed.
		MemoryUnit<EMemoryRatio::Bytes, uintmax_t> MaxSize = MemoryUnit<EMemoryRatio::MiB, uintmax_t>(2);
	};

	struct LogIpcConnectionInfo
	{
		// Name of the IPC socket the engine should connect to.
		std::string Name = "wondermake_logging_connection";
	};

	struct LogIpcSocketInfo
	{
		// Name of the IPC socket that other instances of the engine can connect to.
		std::string Name = "wondermake_logging_connection";
	};

	struct LoggingInfo
	{
		// Name that will appear infront of formatted log messages.
		std::string LoggerName = "WonderMake";

		// Defines the file all logging will be writting to. If not set, no log file will be made.
		std::optional<LogFileInfo> File = LogFileInfo();

		// The list of allowed severities, any log message that uses a severity not listed here will be discarded and not used.
		std::unordered_set<ELogSeverity> AllowedSeverities = { ELogSeverity::Success, ELogSeverity::Info, ELogSeverity::Warning, ELogSeverity::Error };

		// The lowest level allowed to be logged.
		ELogLevel Level = ELogLevel::Normal;

		// This tells the engine to connect to another instance of the engine, and then send all logging to that instance.
		std::optional<LogIpcConnectionInfo> IpcConnection;

		// This is used to open an IPC socket that other instances of the engine can connect to and send its logging, which will be repeated in this instance.
		std::optional<LogIpcSocketInfo> IpcSocket;
	};
	
	struct ConfigurationInfo
	{
		// Path to the json file containing the application configuration overrides. If the path is not absolute, it will be relative to the working directory.
		std::filesystem::path OverrideFileApplication = "overrides.json";

		// Path to the json file containing the device configuration overrides.
		FilePath OverrideFileDevice = FilePath(FilePath::EFolder::Data, "device_settings.json");

		// Path to the json file containing the user configuration overrides.
		FilePath OverrideFileUser = FilePath(FilePath::EFolder::UserData, "user_settings.json");
	};

	struct GraphicsInfo
	{
		// Title used by the Window. Will use Info::ApplicationName if unset.
		std::string WindowTitle;

		// Path to the texture used by Sprites if their set texture is missing.
		FilePath MissingTexture = FilePath(FilePath::EFolder::Bin, "textures/missing_texture.jpg");

		// Path to the node graph used by default when creating a display.
		FilePath DefaultRenderGraphPath = FilePath(FilePath::EFolder::Bin, "node_graphs/render/main_display.json");

		// Path to the directory containing the TTF files used by ImGui.
		FilePath ImguiFontDirectory = FilePath(FilePath::EFolder::Bin, "fonts");
	};

	struct AudioInfo
	{
		// Path to the node graph used to determine what audio is played.
		FilePath MainNodeGraph = FilePath(FilePath::EFolder::Bin, "node_graphs/audio/main.json");
	};

	struct Info
	{
		// Human readable name intended to be the name of the application.
		std::string ApplicationName = "WonderMake";

		// Version of your application. Only used at the start to print the version to the log.
		std::string Version = "1.0.0";

		// Command line args to be passed to the CmdLineArgsSystem, allowing them to be easily accessed by the application. WonderMake itself does not make use of them.
		CmdLineArgs CommandLineArguments = CmdLineArgs(std::vector<std::string>());

		// If true, this will turn off all UI elements, such as the Window itself. Intended for applications that is supposed to run in the background.
		bool Headless = false;

		// Directory path that all files and folders used by the engine will be placed in. Consider using name of the creators followed by the application name. Example: "FantasticGamesCorp/SuperJumperTheGame/"
		std::filesystem::path ProjectFolderNames;

		// All options regarding logging.
		LoggingInfo Logging;

		// All options regarding configuration.
		ConfigurationInfo Configuration;

		// All options regarding graphics. Ignored if Headless is set to true.
		std::optional<GraphicsInfo> Graphics = GraphicsInfo();

		// All options regarding audio. Ignored if Headless is set to true.
		std::optional<AudioInfo> Audio = AudioInfo();
	};

	struct Callbacks
	{
		Closure OnSetup = []() {};
	};

	//wroom wroom
	void Start(Info&& aInfo, Callbacks&& aCallbacks);
};

