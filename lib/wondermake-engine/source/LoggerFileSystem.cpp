#include "wondermake-engine/LoggerFileSystem.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/Time.h"

#include <chrono>
#include <thread>

REGISTER_SYSTEM(LoggerFileSystem);

void LoggerFileSystem::SetLogSizeLimits(MemoryUnit<EMemoryRatio::Bytes, uintmax_t> aTrimSize, MemoryUnit<EMemoryRatio::Bytes, uintmax_t> aMaxSize)
{
	myTrimSize = aTrimSize;
	myMaxSize = aMaxSize;
}

bool LoggerFileSystem::OpenLogFile(FilePath aLogPath)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	Logger::Get().AddLogger(weak_from_this());

	myLogPath = std::move(aLogPath);

	std::filesystem::path directory = myLogPath.Path.parent_path();

	if (!directory.empty())
		std::filesystem::create_directories(directory);

	return ReopenAndTrimFile();
}

void LoggerFileSystem::Print(const SLogLine& aLogLine)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	if (!myFileStream.is_open())
		return;

	myFileStream << Logger::FormatLine(aLogLine) << '\n';

	myFileStream.flush();

	if (myCurrentByteSize > myMaxSize.To<EMemoryRatio::Bytes>())
		(void)ReopenAndTrimFile();
}

bool LoggerFileSystem::ReopenAndTrimFile()
{
	myFileStream.flush();

	myFileStream.close();

	if (!std::filesystem::exists(myLogPath))
	{
		myCurrentByteSize = 0;
		myFileStream.open(myLogPath);

		return myFileStream.is_open();
	}

	const uintmax_t currentLogSize = std::filesystem::file_size(myLogPath);

	if (currentLogSize <= myTrimSize.To<EMemoryRatio::Bytes>())
	{
		myCurrentByteSize = currentLogSize;
		myFileStream.open(myLogPath, std::fstream::app);

		return myFileStream.is_open();
	}

	const uintmax_t bytesToRemove = currentLogSize - myTrimSize.To<EMemoryRatio::Bytes>();
	const uintmax_t bytesToKeep = myTrimSize.To<EMemoryRatio::Bytes>();

	std::ifstream readStream(myLogPath, std::ios::binary);
	std::vector<char> logData(bytesToKeep, '\0');

	readStream.seekg(bytesToRemove, std::ios::beg);

	readStream.read(logData.data(), bytesToKeep);

	readStream.close();

	std::ofstream writeStream(myLogPath, std::ios::binary);

	writeStream.write(logData.data(), logData.size());
	myCurrentByteSize = logData.size();

	writeStream.flush();

	writeStream.close();

	myFileStream.open(myLogPath, std::fstream::app);

	return myFileStream.is_open();
}
