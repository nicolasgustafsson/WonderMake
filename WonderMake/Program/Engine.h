#pragma once

#include "WonderMakeBase/LoggerTypes.h"

#include "WonderMakeUtility/MemoryUnit.h"
#include "WonderMakeUtility/UniqueFunction.h"
#include "WonderMakeUtility/Typedefs.h"

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
		MemoryUnit<EMemoryRatio::Bytes, uintmax_t> MaxSize = MemoryUnit<EMemoryRatio::MiB, uintmax_t>(2);;
	};

	struct LoggingInfo
	{
		// Defines the file all logging will be writting to. If not set, no log file will be made.
		std::optional<LogFileInfo> File;

		// The list of allowed severities, any log message that uses a severity not listed here will be discarded and not used.
		std::unordered_set<ELogSeverity> AllowedSeverities = { ELogSeverity::Success, ELogSeverity::Info, ELogSeverity::Warning, ELogSeverity::Error };

		// The lowest level allowed to be logged.
		ELogLevel Level = ELogLevel::Normal;
	};

	struct Info
	{
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

