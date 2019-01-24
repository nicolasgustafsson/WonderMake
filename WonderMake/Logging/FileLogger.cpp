#include "stdafx.h"

#include "Logging/FileLogger.h"

constexpr auto LogFilePath = "log.txt";

FileLogger::FileLogger()
	: mySubscriber(ERoutineId::File,
		BindHelper(&FileLogger::OnLogMessage, this))
	, myFileStream(LogFilePath, std::fstream::app)
{}

void FileLogger::OnLogMessage(const SLogMessage& aMessage)
{
	myFileStream << aMessage.LogText << '\n';
	myFileStream.flush();
}
