#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/Logger.h"

#include "Logger.cpp"

class LoggerMock
	: public LoggerBase
{
public:
	MOCK_METHOD(void, Print, (
		ELogSeverity aSeverity,
		ELogLevel aLevel,
		std::string aLogMessage), (override));
};

inline constexpr auto locDummySeverity = ELogSeverity::Info;
inline constexpr auto locDummyLevel = ELogLevel::Normal;
inline constexpr auto locDummyFile = "Dummy File";
inline constexpr auto locDummyLine = 1234;
inline constexpr auto locDummyLoggerName = "Logger";
inline constexpr auto locDummyTimepoint = "01-01-1970 00:00:00";
inline constexpr auto locDummyThreadHash = 0xabcd1234;
inline constexpr auto locDummyMessage = "Dummy Message";

TEST(LoggingTests, print_args_are_passed_to_loggers)
{
	auto loggerMockA = std::make_shared<StrictMock<LoggerMock>>();
	auto loggerMockB = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, locDummyLevel);

	logging.AddLogger(loggerMockA);
	logging.AddLogger(loggerMockB);

	EXPECT_CALL(*loggerMockA, Print(locDummySeverity, locDummyLevel, Eq(locDummyMessage)));
	EXPECT_CALL(*loggerMockB, Print(locDummySeverity, locDummyLevel, Eq(locDummyMessage)));

	logging.Print(locDummySeverity, locDummyLevel, locDummyMessage);
}

TEST(LoggingTests, print_args_are_filtered_based_on_allowed_severities)
{
	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ ELogSeverity::Success, ELogSeverity::Error }, locDummyLevel);

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(ELogSeverity::Success, _, _));
	EXPECT_CALL(*loggerMock, Print(ELogSeverity::Error, _, _));

	logging.Print(ELogSeverity::Success,	locDummyLevel, locDummyMessage);
	logging.Print(ELogSeverity::Info,		locDummyLevel, locDummyMessage);
	logging.Print(ELogSeverity::Warning,	locDummyLevel, locDummyMessage);
	logging.Print(ELogSeverity::Error,		locDummyLevel, locDummyMessage);
}

TEST(LoggingTests, print_args_are_filtered_based_on_min_level)
{
	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, ELogLevel::Normal);

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(_, ELogLevel::Normal, _));
	EXPECT_CALL(*loggerMock, Print(_, ELogLevel::Priority, _));

	logging.Print(locDummySeverity, ELogLevel::Debug,		locDummyMessage);
	logging.Print(locDummySeverity, ELogLevel::Verbose,		locDummyMessage);
	logging.Print(locDummySeverity, ELogLevel::Normal,		locDummyMessage);
	logging.Print(locDummySeverity, ELogLevel::Priority,	locDummyMessage);
}

TEST(LoggingTests, logger_formatline)
{
	static constexpr auto expectedMessage = "[Logger]       01-01-1970 00:00:00 [0xabcd1234]         Info    Dummy File(1234)               Dummy Message";

	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logger;

	logger.SetLoggerName(locDummyLoggerName);

	logger.FormatLine(locDummySeverity, locDummyMessage, locDummyFile, locDummyLine, locDummyTimepoint, locDummyThreadHash);
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
