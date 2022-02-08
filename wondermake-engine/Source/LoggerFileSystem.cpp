#include "WonderMakeEngine/LoggerFileSystem.h"

#include "wondermake-io/FileSystem.h"

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

bool LoggerFileSystem::OpenLogFile(std::filesystem::path aRelativeFolder, std::filesystem::path aFileName)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	Logger::Get().AddLogger(shared_from_this());

	auto&& fileSystem = Get<FileSystem>();

	auto pathOpt = fileSystem.GetFolderLocation(FolderLocation::UserData);

	if (!pathOpt)
		return false;

	const auto pathDir	= *pathOpt / aRelativeFolder;
	myLogPath = *pathOpt / aRelativeFolder / aFileName;

	std::filesystem::create_directories(pathDir);

	return ReopenAndTrimFile();
}

void LoggerFileSystem::Print(ELogSeverity /*aSeverity*/, ELogLevel /*aLevel*/, std::string aLogMessage)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	if (!myFileStream.is_open())
		return;

	myFileStream << std::move(aLogMessage) << '\n';

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
