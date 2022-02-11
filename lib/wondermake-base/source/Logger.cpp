#include "wondermake-base/Logger.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Time.h"

#include <thread>

std::string SeverityToString(ELogSeverity aSeverity)
{
	switch (aSeverity)
	{
	case ELogSeverity::Success:	return "Success";
	case ELogSeverity::Info:	return "Info";
	case ELogSeverity::Warning:	return "Warning";
	case ELogSeverity::Error:	return "Error";
	}

	return "Unknown value: " + std::to_string(static_cast<std::underlying_type_t<ELogSeverity>>(aSeverity));
}

Logger::Builder::Builder(Logger& aLogger, ELogSeverity aSeverity, ELogLevel aLevel, std::string_view aFile, u64 aLine, const std::string& aLoggerName, std::chrono::time_point<std::chrono::system_clock> aTimepoint, size_t aThreadHash)
	: myLogger(aLogger)
	, mySeverity(aSeverity)
	, myLevel(aLevel)
{
	auto index = aFile.find_last_of("/");

	if (index == std::string_view::npos)
		index = aFile.find_last_of("\\");

	if (index != std::string_view::npos)
		aFile = aFile.substr(index + 1);

	std::stringstream threadHashStream;

	threadHashStream << "[0x" << std::hex << aThreadHash << ']';

	myStringStream
		<< std::setw(15) << std::setfill(' ') << std::left
		<< ("[" + aLoggerName + "] ")
		<< TimePointToString(aTimepoint) << ' '
		<< std::setw(20) << std::setfill(' ') << std::left
		<< std::move(threadHashStream).str() << ' '
		<< std::setw(7) << std::setfill(' ') << std::left
		<< SeverityToString(aSeverity) << ' '
		<< std::setw(30) << std::setfill(' ') << std::left
		<< (static_cast<std::string>(aFile) + '(' + std::to_string(aLine) + ')') << ' ';
}

Logger::Builder::Builder(Logger& aLogger, ELogSeverity aSeverity, ELogLevel aLevel, std::string_view aFile, u64 aLine)
	: Builder(aLogger, aSeverity, aLevel, aFile, aLine, aLogger.GetLoggerName(), std::chrono::system_clock::now(), std::hash<std::thread::id>{}(std::this_thread::get_id()))
{}

Logger::Builder::~Builder()
{
	myLogger.Print(mySeverity, myLevel, std::move(myStringStream).str());
}

void Logger::SetLoggerName(std::string aName)
{
	myLoggerName = std::move(aName);
}

const std::string& Logger::GetLoggerName() const
{
	return myLoggerName;
}

void Logger::SetFilters(std::unordered_set<ELogSeverity> aAllowedSeverities, ELogLevel aMinLevel)
{
	myAllowedSeverities = std::move(aAllowedSeverities);
	myMinLevel = aMinLevel;
}

void Logger::AddLogger(std::weak_ptr<LoggerBase> aLogger)
{
	myLoggers.emplace_back(std::move(aLogger));
}

void Logger::Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage)
{
	using LevelType = std::underlying_type_t<ELogLevel>;

	if (static_cast<LevelType>(myMinLevel) > static_cast<LevelType>(aLevel)
		|| myAllowedSeverities.find(aSeverity) == myAllowedSeverities.cend())
		return;

	for (auto&& logger : myLoggers)
	{
		auto ptr = logger.lock();

		if (!ptr)
			continue;

		ptr->Print(aSeverity, aLevel, aLogMessage);
	}

	const auto it = std::partition(myLoggers.begin(), myLoggers.end(), [](const auto& aWeakPtr)
		{
			return aWeakPtr.lock() != nullptr;
		});

	myLoggers.erase(it, myLoggers.end());
}
