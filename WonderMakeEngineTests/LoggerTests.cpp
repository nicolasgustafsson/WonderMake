#include "WonderMakeTestsCommon/GTestInclude.h"

#include "WonderMakeEngine/Logger.h"

class LoggerMock
	: public LoggerBase
{
public:
	MOCK_METHOD(void, Print, (
		ELogSeverity aSeverity,
		ELogLevel aLevel,
		std::string aLogMessage), (override));
};

constexpr auto locDummySeverity = ELogSeverity::Info;
constexpr auto locDummyLevel = ELogLevel::Normal;
constexpr auto locDummyFile = "Dummy File";
constexpr auto locDummyLine = 1234;
constexpr auto locDummyLoggerName = "Logger";
constexpr auto locDummyTimepoint = std::chrono::time_point<std::chrono::system_clock>();
constexpr auto locDummyThreadHash = 0xabcd1234;
constexpr auto locDummyMessage = "Dummy Message";

TEST(LoggingTests, print_args_are_passed_to_loggers)
{
	auto loggerMockA = std::make_shared<StrictMock<LoggerMock>>();
	auto loggerMockB = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, { locDummyLevel });

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

	logging.SetFilters({ ELogSeverity::Success, ELogSeverity::Error }, { locDummyLevel });

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

TEST(LoggingTests, builder_passes_correctly_formatted_message_to_logging)
{
	enum class TestEnum
	{
		CorrectValue = 5678
	};

	constexpr auto messageString = "Test Value: ";
	constexpr auto messageEnum = TestEnum::CorrectValue;
	constexpr auto expectedMessage = "[Logger]       01-01-1970 01:00:00 [0xabcd1234]         Info    Dummy File(1234)               Test Value: 5678";

	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, { locDummyLevel });

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(locDummySeverity, locDummyLevel, Eq(expectedMessage)));

	{
		Logger::Builder builder(logging, locDummySeverity, locDummyLevel, locDummyFile, locDummyLine, locDummyLoggerName, locDummyTimepoint, locDummyThreadHash);

		builder << messageString << messageEnum;
	}
}

TEST(LoggingTests, builder_trims_filename_before_passing_it_to_logger)
{
	constexpr auto originalFileName = "C:\\Program Files\\Tests/LoggerTests.cpp";
	constexpr auto expectedFileName = "LoggerTests.cpp";

	constexpr auto expectedMessage = "[Logger]       01-01-1970 01:00:00 [0xabcd1234]         Info    LoggerTests.cpp(1234)          Dummy Message";

	auto loggerMock = std::make_shared<StrictMock<LoggerMock>>();

	Logger logging;

	logging.SetFilters({ locDummySeverity }, { locDummyLevel });

	logging.AddLogger(loggerMock);

	EXPECT_CALL(*loggerMock, Print(_, _, Eq(expectedMessage)));

	{
		Logger::Builder builder(logging, locDummySeverity, locDummyLevel, originalFileName, locDummyLine, locDummyLoggerName, locDummyTimepoint, locDummyThreadHash);

		builder << locDummyMessage;
	}
}
