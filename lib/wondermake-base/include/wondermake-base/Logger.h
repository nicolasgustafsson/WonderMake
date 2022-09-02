#pragma once

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/LogTag.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Singleton.h"
#include "wondermake-utility/Typedefs.h"

#include <chrono>
#include <memory>
#include <source_location>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace SystemTraits
{
	struct Logger : public Trait {};
}

using STLogger = SystemTraits::Logger;

class LoggerBase
{
public:
	virtual ~LoggerBase() = default;

	virtual void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) = 0;

};

template<typename TType>
concept CWmLoggable = requires(std::ostream& aStream, TType&& aType)
{
	{ WmLogStream(aStream, std::forward<TType>(aType)) };
};

class Logger final
	: public Singleton<Logger>
{
public:
	struct SLogText
	{
		std::string Line;
	};

	std::string FormatLine(ELogSeverity aSeverity, std::string aMessage, std::string_view aFile, u64 aLine, std::string aTimestamp, size_t aThreadHash);
	std::string FormatLine(ELogSeverity aSeverity, std::string aMessage, std::string_view aFile, u64 aLine);

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

template<typename TType>
inline static void WmLogStream(std::ostream& aStream, const TType& aArgument)
	requires(std::is_enum_v<std::decay_t<TType>>)
{
	aStream << static_cast<std::underlying_type_t<std::decay_t<TType>>>(aArgument);
}

void WmLog(
	[[maybe_unused]] Logger::SLogText aText,
	[[maybe_unused]] ELogSeverity aSeverity,
	[[maybe_unused]] ELogLevel aLevel,
	[[maybe_unused]] Logger& aLogger = Logger::Get(),
	[[maybe_unused]] std::source_location aSourceLocation = std::source_location::current());

inline void WmLogSuccess(Logger::SLogText aText, Logger& aLogger = Logger::Get(), std::source_location aSourceLocation = std::source_location::current())
{
	WmLog(std::move(aText), ELogSeverity::Success, ELogLevel::Normal, aLogger, aSourceLocation);
}
inline void WmLogInfo(Logger::SLogText aText, Logger& aLogger = Logger::Get(), std::source_location aSourceLocation = std::source_location::current())
{
	WmLog(std::move(aText), ELogSeverity::Info, ELogLevel::Normal, aLogger, aSourceLocation);
}
inline void WmLogWarning(Logger::SLogText aText, Logger& aLogger = Logger::Get(), std::source_location aSourceLocation = std::source_location::current())
{
	WmLog(std::move(aText), ELogSeverity::Warning, ELogLevel::Normal, aLogger, aSourceLocation);
}
inline void WmLogError(Logger::SLogText aText, Logger& aLogger = Logger::Get(), std::source_location aSourceLocation = std::source_location::current())
{
	WmLog(std::move(aText), ELogSeverity::Error, ELogLevel::Normal, aLogger, aSourceLocation);
}

class LogStream
{
public:
	template<typename TType>
	inline [[nodiscard]] LogStream&& operator<<(TType&& aValue) &&
	{
		if constexpr (CWmLoggable<TType>)
			WmLogStream(myStream, std::forward<TType>(aValue));
		else
			myStream << std::forward<TType>(aValue);

		return std::move(*this);
	}

	inline [[nodiscard]] operator Logger::SLogText() &&
	{
		return { std::move(myStream).str() };
	}

private:
	std::stringstream myStream;

};

template<bool TCanStreamTag, size_t TSize>
class FixedSizeLogStream
{
public:
	inline constexpr FixedSizeLogStream(const FixedSizeString<TSize>& aString) noexcept
		: myStream(aString)
	{}

	template<size_t TTagSize>
	inline constexpr [[nodiscard]] auto operator<<(const LogTag<TTagSize>& aValue) noexcept
		requires(TCanStreamTag)
	{
		return ::FixedSizeLogStream<true, TSize + LogTag<TTagSize>::FormattedSize>(myStream + aValue.GetFormattedString());
	}
	template<CCanConcatWithFixedSizeString TType>
	inline constexpr [[nodiscard]] auto operator<<(const TType& aValue) noexcept
	{
		return ::FixedSizeLogStream<false, decltype(myStream + aValue)::Size>(myStream + aValue);
	}

	template<typename TType>
	inline [[nodiscard]] LogStream operator<<(TType&& aValue)
		requires(!CCanConcatWithFixedSizeString<TType>)
	{
		return (LogStream() << myStream << std::forward<TType>(aValue));
	}

	inline constexpr [[nodiscard]] operator Logger::SLogText()
		requires(!TCanStreamTag)
	{
		return { myStream.ToString() };
	}

private:
	FixedSizeString<TSize> myStream;

};

template<size_t TSize, typename TType>
inline constexpr [[nodiscard]] auto operator<<(const LogTag<TSize>& aLogTag, TType&& aValue)
{
	return (FixedSizeLogStream<true, LogTag<TSize>::FormattedSize>(aLogTag.GetFormattedString()) << std::forward<TType>(aValue));
}
