#pragma once

#include "WonderMakeBase/LoggerTypes.h"

#include "WonderMakeUtility/Typedefs.h"

#include <span>
#include <vector>

struct LoggerRemoteMessageType
{
	ELogSeverity Severity = ELogSeverity::Error;
	ELogLevel Level = ELogLevel::Normal;
	std::string Message;

	std::vector<u8> Serialize() const noexcept;
	size_t Deserialize(const std::span<u8>& aData) noexcept;
};
