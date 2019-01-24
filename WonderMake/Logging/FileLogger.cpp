#include "stdafx.h"

#include "Logging/FileLogger.h"

#include "Utilities/OS.h"

#include <algorithm>
#include <filesystem>

constexpr auto LogFileDirectory = "Logs";
constexpr auto LogFileSuffix = ".txt";

FileLogger::FileLogger()
	: mySubscriber(ERoutineId::File,
		BindHelper(&FileLogger::OnLogMessage, this))
{
	std::filesystem::path logDir = std::filesystem::current_path() / LogFileDirectory;
	if (!std::filesystem::is_directory(logDir))
	{
		std::filesystem::create_directories(logDir);
	}

	std::string fileName;
	GetDateTime(fileName);
	fileName += LogFileSuffix;

	std::filesystem::path filePath = logDir / fileName;
	myFileStream.open(filePath, std::fstream::out | std::fstream::app);
	if (!myFileStream)
	{
		WmLog(TagError, "Failed to open log file.");
	}
}

void FileLogger::OnLogMessage(const SLogMessage& aMessage)
{
	myFileStream << aMessage.LogText << '\n';
	myFileStream.flush();
}
