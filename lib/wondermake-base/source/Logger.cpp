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

std::string Logger::FormatLine(const SLogLine& aLogLine)
{
	std::stringstream threadHashStream;

	threadHashStream << "[0x" << std::hex << aLogLine.ThreadHash << ']';

	std::stringstream ss;

	ss
		<< std::setw(15) << std::setfill(' ') << std::left
		<< ("[" + aLogLine.LoggerName + "] ")
		<< '[' << aLogLine.ProcessId.Id().ToFixedSizeString() << "] "
		<< std::setw(32) << std::setfill(' ') << std::left
		<< aLogLine.Timestamp << ' '
		<< std::setw(20) << std::setfill(' ') << std::left
		<< std::move(threadHashStream).str() << ' '
		<< std::setw(7) << std::setfill(' ') << std::left
		<< SeverityToString(aLogLine.Severity) << ' '
		<< std::setw(30) << std::setfill(' ') << std::left
		<< (aLogLine.File + '(' + std::to_string(aLogLine.Line) + ')') << ' '
		<< aLogLine.Message;

	return std::move(ss).str();
}

void Logger::SetProcessId(ProcessId aId) noexcept
{
	myProcessId = aId;
}

[[nodiscard]] ProcessId Logger::GetProcessId() const noexcept
{
	return myProcessId;
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

void Logger::Print(const SLogLine& aLogLine)
{
	using LevelType = std::underlying_type_t<ELogLevel>;

	if (static_cast<LevelType>(myMinLevel) > static_cast<LevelType>(aLogLine.Level)
		|| myAllowedSeverities.find(aLogLine.Severity) == myAllowedSeverities.cend())
		return;

	for (auto&& logger : myLoggers)
	{
		auto ptr = logger.lock();

		if (!ptr)
			continue;

		ptr->Print(aLogLine);
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
		std::string_view file = aSourceLocation.file_name();

		auto index = file.find_last_of("/");

		if (index == std::string_view::npos)
			index = file.find_last_of("\\");

		if (index != std::string_view::npos)
			file = file.substr(index + 1);

		SLogLine logLine
		{
			aSeverity,
			aLevel,
			std::move(aText.Line),
			static_cast<std::string>(file),
			static_cast<u64>(aSourceLocation.line()),
			TimePointToISO8601(std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())).UnwrapOr("[Time-Error]"),
			std::hash<std::thread::id>{}(std::this_thread::get_id()),
			aLogger.GetLoggerName(),
			aLogger.GetProcessId()
		};

		aLogger.Print(logLine);
	}
}
