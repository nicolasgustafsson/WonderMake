#pragma once

#include "WonderMakeBase/LoggerTypes.h"

#include "WonderMakeUtility/Result.h"
#include "WonderMakeUtility/Typedefs.h"

#include <span>
#include <vector>

struct LoggerRemoteMessageType
{
	ELogSeverity Severity = ELogSeverity::Error;
	ELogLevel Level = ELogLevel::Normal;
	std::string Message;

	static std::vector<u8> Serialize(const LoggerRemoteMessageType& aMessage) noexcept;
	static Result<decltype(Failure), std::pair<LoggerRemoteMessageType, size_t>> Deserialize(std::span<const u8> aData) noexcept;
};
