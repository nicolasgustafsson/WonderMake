#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/Logger.h"

#include "Logger.cpp"

MATCHER_P(SLogLineMatcher, aLogLine, "")
{
	return ExplainMatchResult(
		AllOf(
			Field("Severity",	&SLogLine::Severity,	aLogLine.Severity),
			Field("Level",		&SLogLine::Level,		aLogLine.Level),
			Field("Message",	&SLogLine::Message,		aLogLine.Message),
			Field("File",		&SLogLine::File,		aLogLine.File),
			Field("Line",		&SLogLine::Line,		aLogLine.Line),
			Field("Timestamp",	&SLogLine::Timestamp,	aLogLine.Timestamp),
			Field("ThreadHash",	&SLogLine::ThreadHash,	aLogLine.ThreadHash),
			Field("LoggerName",	&SLogLine::LoggerName,	aLogLine.LoggerName)),
		arg, result_listener);
}

class LoggerMock
	: public LoggerBase
{
public:
	MOCK_METHOD(void, Print, (const SLogLine&), (override));
};

inline constexpr auto locDummySeverity = ELogSeverity::Info;
inline constexpr auto locDummyLevel = ELogLevel::Normal;
inline constexpr auto locDummyFile = "Dummy File";
inline constexpr auto locDummyLine = 1234;
inline constexpr auto locDummyTimepoint = "01-01-1970 00:00:00";
inline constexpr auto locDummyThreadHash = 0xabcd1234;
inline constexpr auto locDummyLoggerName = "Logger";
inline constexpr auto locDummyProcessId = *Guid::Parse("a4930e6b-965f-4341-ba4b-18b569e82fa8");
inline constexpr auto locDummyMessage = "Dummy Message";

TEST(LoggingTests, print_args_are_passed_to_loggers)
{
	auto loggerMockA = std::make_shared<StrictMock<LoggerMock>>();
	auto loggerMockB = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, locDummyLevel);

	logging.AddLogger(loggerMockA);
	logging.AddLogger(loggerMockB);

	const SLogLine logLine
	{
		locDummySeverity,
		locDummyLevel,
		locDummyMessage
	};

	EXPECT_CALL(*loggerMockA, Print(SLogLineMatcher(logLine)));
	EXPECT_CALL(*loggerMockB, Print(SLogLineMatcher(logLine)));

	logging.Print(logLine);
}

TEST(LoggingTests, print_args_are_filtered_based_on_allowed_severities)
{
	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ ELogSeverity::Success, ELogSeverity::Error }, locDummyLevel);

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(SLogLineMatcher(SLogLine{ ELogSeverity::Success,	locDummyLevel, locDummyMessage })));
	EXPECT_CALL(*loggerMock, Print(SLogLineMatcher(SLogLine{ ELogSeverity::Error,	locDummyLevel, locDummyMessage })));

	logging.Print(SLogLine{ ELogSeverity::Success,	locDummyLevel, locDummyMessage });
	logging.Print(SLogLine{ ELogSeverity::Info,		locDummyLevel, locDummyMessage });
	logging.Print(SLogLine{ ELogSeverity::Warning,	locDummyLevel, locDummyMessage });
	logging.Print(SLogLine{ ELogSeverity::Error,	locDummyLevel, locDummyMessage });
}

TEST(LoggingTests, print_args_are_filtered_based_on_min_level)
{
	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, ELogLevel::Normal);

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(SLogLineMatcher(SLogLine{ locDummySeverity, ELogLevel::Normal,	locDummyMessage })));
	EXPECT_CALL(*loggerMock, Print(SLogLineMatcher(SLogLine{ locDummySeverity, ELogLevel::Priority,	locDummyMessage })));

	logging.Print(SLogLine{ locDummySeverity, ELogLevel::Debug,		locDummyMessage });
	logging.Print(SLogLine{ locDummySeverity, ELogLevel::Verbose,	locDummyMessage });
	logging.Print(SLogLine{ locDummySeverity, ELogLevel::Normal,	locDummyMessage });
	logging.Print(SLogLine{ locDummySeverity, ELogLevel::Priority,	locDummyMessage });
}

TEST(LoggingTests, logger_formatline)
{
	static constexpr auto expectedLine = "[Logger]       [a4930e6b-965f-4341-ba4b-18b569e82fa8] 01-01-1970 00:00:00              [0xabcd1234]         Info    Dummy File(1234)               Dummy Message";
	const SLogLine logLine
	{
		locDummySeverity,
		locDummyLevel,
		locDummyMessage,
		locDummyFile,
		locDummyLine,
		locDummyTimepoint,
		locDummyThreadHash,
		locDummyLoggerName,
		locDummyProcessId
	};

	const auto formattedLine = Logger::FormatLine(logLine);

	EXPECT_EQ(formattedLine, expectedLine);
}

TEST(LoggingTests, logger_fixedsizelogstream_can_stream_chararrays)
{
	static constexpr auto tag				= MakeLogTag("Test");
	static constexpr auto message			= MakeFixedSizeString("Hello!");
	static constexpr char expectedLine[]	= "[Test] Hello!";

	static constexpr size_t expectedSize	= sizeof(expectedLine) - 1;

	FixedSizeLogStream<false, expectedSize> stream = tag << message;

	Logger::SLogText text = stream;

	EXPECT_EQ(text.Line, expectedLine);
}

TEST(LoggingTests, logger_fixedsizelogstream_can_stream_logtags)
{
	static constexpr auto tagFirst			= MakeLogTag("First");
	static constexpr auto tagSecond			= MakeLogTag("Second");
	static constexpr auto message			= MakeFixedSizeString("Hello!");
	static constexpr char expectedLine[]	= "[First] [Second] Hello!";

	static constexpr size_t expectedSize	= sizeof(expectedLine) - 1;

	FixedSizeLogStream<false, expectedSize> stream = tagFirst << tagSecond << message;

	Logger::SLogText text = stream;

	EXPECT_EQ(text.Line, expectedLine);
}
