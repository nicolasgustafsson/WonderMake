#pragma once

#include "wondermake-base/LoggerTypes.h"

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

	struct Info
	{
		// Human readable name intended to be the name of the application.
		std::string ApplicationName = "WonderMake";

		std::vector<std::wstring> CommandLineArguments;
		bool Headless = true;
		std::filesystem::path ProjectFolderNames;
		LoggingInfo Logging;
	};

	struct Callbacks
	{
		Closure OnSetup;
	};

	//wroom wroom
	void Start(Info&& aInfo, Callbacks&& aCallbacks);
};

