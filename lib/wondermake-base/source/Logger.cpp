#include "wondermake-base/Logger.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Time.h"

#include <thread>

#ifdef WM_SUPPRESS_LOGGER
inline constexpr bool IsWmLoggingSuppressed = true;
#else
inline constexpr bool IsWmLoggingSuppressed = false;
#endif

std::string SeverityToString(ELogSeverity aSeverity)
{
	switch (aSeverity)
	{
	case ELogSeverity::Success:	return "Success";
	case ELogSeverity::Info:	return "Info";
	case ELogSeverity::Warning:	return "Warning";
	case ELogSeverity::Error:	return "Error";
	}

	return "Unknown Severity(" + std::to_string(static_cast<std::underlying_type_t<ELogSeverity>>(aSeverity)) + ')';
}

std::string Logger::FormatLine(ELogSeverity aSeverity, std::string aMessage, std::string_view aFile, u64 aLine, std::string aTimestamp, size_t aThreadHash)
{
	auto index = aFile.find_last_of("/");

	if (index == std::string_view::npos)
		index = aFile.find_last_of("\\");

	if (index != std::string_view::npos)
		aFile = aFile.substr(index + 1);

	std::stringstream threadHashStream;

	threadHashStream << "[0x" << std::hex << aThreadHash << ']';

	std::stringstream ss;

	ss
		<< std::setw(15) << std::setfill(' ') << std::left
		<< ("[" + GetLoggerName() + "] ")
		<< std::move(aTimestamp) << ' '
		<< std::setw(20) << std::setfill(' ') << std::left
		<< std::move(threadHashStream).str() << ' '
		<< std::setw(7) << std::setfill(' ') << std::left
		<< SeverityToString(aSeverity) << ' '
		<< std::setw(30) << std::setfill(' ') << std::left
		<< (static_cast<std::string>(aFile) + '(' + std::to_string(aLine) + ')') << ' ';

	return std::move(ss).str();
}

std::string Logger::FormatLine(ELogSeverity aSeverity, std::string aMessage, std::string_view aFile, u64 aLine)
{
	return FormatLine(
		aSeverity,
		std::move(aMessage),
		aFile,
		aLine,
		TimePointToISO8601(std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())).UnwrapOr("[Time-Error]"),
		std::hash<std::thread::id>{}(std::this_thread::get_id()));
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

void WmLog(
	[[maybe_unused]] Logger::SLogText aText,
	[[maybe_unused]] ELogSeverity aSeverity,
	[[maybe_unused]] ELogLevel aLevel,
	[[maybe_unused]] Logger& aLogger,
	[[maybe_unused]] std::source_location aSourceLocation)
{
	if constexpr (!IsWmLoggingSuppressed)
	{
		auto logLine = aLogger.FormatLine(aSeverity, std::move(aText.Line), aSourceLocation.file_name(), static_cast<u64>(aSourceLocation.line()));

		aLogger.Print(aSeverity, aLevel, std::move(logLine));
	}
}
