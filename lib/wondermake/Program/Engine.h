#pragma once

#include "wondermake-base/LoggerTypes.h"

#include "wondermake-utility/CmdLineArgs.h"
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
		// The log file's directory relative to the project folder inside user data directory.
		std::filesystem::path Path;

		// Name of the log file, including file extension.
		std::filesystem::path Filename;

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
		std::optional<LogFileInfo> File;

		// The list of allowed severities, any log message that uses a severity not listed here will be discarded and not used.
		std::unordered_set<ELogSeverity> AllowedSeverities = { ELogSeverity::Success, ELogSeverity::Info, ELogSeverity::Warning, ELogSeverity::Error };

		// The lowest level allowed to be logged.
		ELogLevel Level = ELogLevel::Normal;

		// This tells the engine to connect to another instance of the engine, and then send all logging to that instance.
		std::optional<LogIpcConnectionInfo> IpcConnection;

		// This is used to open an IPC socket that other instances of the engine can connect to and send its logging, which will be repeated in this instance.
		std::optional<LogIpcSocketInfo> IpcSocket;
	};
	
	// EOverrideFileUserLocation represents the directory location that can be used by the ConfigurationInfo.OverrideFileUser field.
	enum class EOverrideFileUserLocation
	{
		// The user directory. On Windows this is usually inside the documents folder.
		User,
		// The userdata directory. On Windows this is usually inside the appdata folder.
		UserData
	};

	struct ConfigurationInfo
	{
		// Path to the json file containing the application configuration overrides. If the path is not absolute, it will be relative to the working directory.
		std::filesystem::path OverrideFileApplication = "overrides.json";

		// Path to the json file containing the device configuration overrides. If the path is not absolute, it will be relative to the data directory.
		std::filesystem::path OverrideFileDevice = "device_settings.json";

		// Path to the json file containing the user configuration overrides. If the path is not absolute, it will be relative to the directory set by UserOverrideFileLocation.
		std::filesystem::path OverrideFileUser = "user_settings.json";

		// This sets the directory location of OverrideFileUser. Is ignored if OverrideFileUser is set to an absolute path.
		EOverrideFileUserLocation OverrideFileUserLocation = EOverrideFileUserLocation::UserData;
	};

	struct Info
	{
		// Human readable name intended to be the name of the application.
		std::string ApplicationName = "WonderMake";

		// Command line args to be passed to the CmdLineArgsSystem, allowing them to be easily accessed by the application. WonderMake itself does not make use of them.
		CmdLineArgs CommandLineArguments = CmdLineArgs(std::vector<std::string>());

		// If true, this will turn off all UI elements, such as the Window itself. Intended for applications that is supposed to run in the background.
		bool Headless = true;

		// Directory path that all files and folders used by the engine will be placed in. Consider using name of the creators followed by the application name. Example: "FantasticGamesCorp/SuperJumperTheGame/"
		std::filesystem::path ProjectFolderNames;

		// All options regarding logging.
		LoggingInfo Logging;

		// All options regarding configuration.
		ConfigurationInfo Configuration;
	};

	struct Callbacks
	{
		Closure OnSetup;
	};

	//wroom wroom
	void Start(Info&& aInfo, Callbacks&& aCallbacks);
};

