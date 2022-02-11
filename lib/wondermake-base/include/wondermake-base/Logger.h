#pragma once

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Singleton.h"
#include "wondermake-utility/Typedefs.h"

#include <chrono>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace SystemTraits
{
	struct Logger : public Trait {};
}

using STLogger = SystemTraits::Logger;

std::string SeverityToString(ELogSeverity aSeverity);

class LoggerBase
{
public:
	virtual ~LoggerBase() = default;

	virtual void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) = 0;

};

class Logger final
	: public Singleton<Logger>
{
public:
	class Builder
	{
	public:
		Builder(Logger& aLogger, ELogSeverity aSeverity, ELogLevel aLevel, std::string_view aFile, u64 aLine, const std::string& aLoggerName, std::chrono::time_point<std::chrono::system_clock> aTimepoint, size_t aThreadHash);
		Builder(Logger& aLogger, ELogSeverity aSeverity, ELogLevel aLevel, std::string_view aFile, u64 aLine);
		~Builder();

		template<typename T>
		inline Builder& operator<<(const T& aValue) requires(!std::is_enum_v<T>)
		{
			myStringStream << aValue;

			return *this;
		}
		template<typename T>
		inline Builder& operator<<(const T& aValue) requires(std::is_enum_v<T>)
		{
			myStringStream << static_cast<std::underlying_type_t<T>>(aValue);

			return *this;
		}

		template<typename... TArgs>
		inline static void StreamHelper(Logger& aLogger, ELogSeverity aSeverity, ELogLevel aLevel, std::string_view aFile, u64 aLine, TArgs&&... aArgs)
		{
			Builder builder(aLogger, aSeverity, aLevel, aFile, aLine);

			const auto stream = [&builder](auto&& aArg)
			{
				builder << std::forward<decltype(aArg)>(aArg);

				return true;
			};

			(stream(std::forward<TArgs>(aArgs)) && ...);
		}

	private:
		Logger& myLogger;
		ELogSeverity mySeverity = ELogSeverity::Error;
		ELogLevel myLevel = ELogLevel::Priority;

		std::stringstream myStringStream;

	};

	void SetLoggerName(std::string aName);
	const std::string& GetLoggerName() const;

	void SetFilters(std::unordered_set<ELogSeverity> aAllowedSeverities, ELogLevel aMinLevel);
	
	void AddLogger(std::weak_ptr<LoggerBase> aLogger);

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage);

private:
	std::string myLoggerName;
	std::unordered_set<ELogSeverity> myAllowedSeverities;
	ELogLevel myMinLevel = ELogLevel::Normal;

	std::vector<std::weak_ptr<LoggerBase>> myLoggers;

};

#define WM_LOG(aSeverity, aLevel, ...)	Logger::Builder::StreamHelper(Logger::Get(), ( aSeverity ), ( aLevel ), __FILE__ , __LINE__ , __VA_ARGS__ )

#define WM_LOG_DEBUG_SUCCESS(...)		WM_LOG(ELogSeverity::Success,	ELogLevel::Debug, __VA_ARGS__ )
#define WM_LOG_DEBUG_INFO(...)			WM_LOG(ELogSeverity::Info,		ELogLevel::Debug, __VA_ARGS__ )
#define WM_LOG_DEBUG_WARNING(...)		WM_LOG(ELogSeverity::Warning,	ELogLevel::Debug, __VA_ARGS__ )
#define WM_LOG_DEBUG_ERROR(...)			WM_LOG(ELogSeverity::Error,		ELogLevel::Debug, __VA_ARGS__ )

#define WM_LOG_VERBOSE_SUCCESS(...)		WM_LOG(ELogSeverity::Success,	ELogLevel::Verbose, __VA_ARGS__ )
#define WM_LOG_VERBOSE_INFO(...)		WM_LOG(ELogSeverity::Info,		ELogLevel::Verbose, __VA_ARGS__ )
#define WM_LOG_VERBOSE_WARNING(...)		WM_LOG(ELogSeverity::Warning,	ELogLevel::Verbose, __VA_ARGS__ )
#define WM_LOG_VERBOSE_ERROR(...)		WM_LOG(ELogSeverity::Error,		ELogLevel::Verbose, __VA_ARGS__ )

#define WM_LOG_NORMAL_SUCCESS(...)		WM_LOG(ELogSeverity::Success,	ELogLevel::Normal, __VA_ARGS__ )
#define WM_LOG_NORMAL_INFO(...)			WM_LOG(ELogSeverity::Info,		ELogLevel::Normal, __VA_ARGS__ )
#define WM_LOG_NORMAL_WARNING(...)		WM_LOG(ELogSeverity::Warning,	ELogLevel::Normal, __VA_ARGS__ )
#define WM_LOG_NORMAL_ERROR(...)		WM_LOG(ELogSeverity::Error,		ELogLevel::Normal, __VA_ARGS__ )

#define WM_LOG_PRIORITY_SUCCESS(...)	WM_LOG(ELogSeverity::Success,	ELogLevel::Priority, __VA_ARGS__ )
#define WM_LOG_PRIORITY_INFO(...)		WM_LOG(ELogSeverity::Info,		ELogLevel::Priority, __VA_ARGS__ )
#define WM_LOG_PRIORITY_WARNING(...)	WM_LOG(ELogSeverity::Warning,	ELogLevel::Priority, __VA_ARGS__ )
#define WM_LOG_PRIORITY_ERROR(...)		WM_LOG(ELogSeverity::Error,		ELogLevel::Priority, __VA_ARGS__ )

#define WM_LOG_SUCCESS(...)				WM_LOG_NORMAL_SUCCESS( __VA_ARGS__ )
#define WM_LOG_INFO(...)				WM_LOG_NORMAL_INFO( __VA_ARGS__ )
#define WM_LOG_WARNING(...)				WM_LOG_NORMAL_WARNING( __VA_ARGS__ )
#define WM_LOG_ERROR(...)				WM_LOG_NORMAL_ERROR( __VA_ARGS__ )
