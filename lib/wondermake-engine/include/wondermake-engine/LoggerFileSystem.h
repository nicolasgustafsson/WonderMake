#pragma once

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/MemoryUnit.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>

class FileSystem;

class LoggerFileSystem
	: public System<
		Policy::Set<
			PAdd<FileSystem, PWrite>>,
		STrait::Set<
			STLogger,
			STThreadsafe,
			STSingleton>>
	, public LoggerBase
	, public std::enable_shared_from_this<LoggerFileSystem>
{
public:
	void SetLogSizeLimits(MemoryUnit<EMemoryRatio::Bytes, uintmax_t> aTrimSize, MemoryUnit<EMemoryRatio::Bytes, uintmax_t> aMaxSize);

	bool OpenLogFile(std::filesystem::path aRelativeFolder, std::filesystem::path aFileName);

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

private:
	bool ReopenAndTrimFile();

	std::mutex myMutex;
	std::ofstream myFileStream;

	std::filesystem::path myLogPath;

	uintmax_t myCurrentByteSize = 0;
	MemoryUnit<EMemoryRatio::Bytes, uintmax_t> myTrimSize = MemoryUnit<EMemoryRatio::KiB, uintmax_t>(64);
	MemoryUnit<EMemoryRatio::Bytes, uintmax_t> myMaxSize = MemoryUnit<EMemoryRatio::KiB, uintmax_t>(128);;

};
