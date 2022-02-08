#pragma once

#include "LoggerRemoteMessageType.h"

std::vector<u8> LoggerRemoteMessageType::Serialize(const LoggerRemoteMessageType& aMessage) noexcept
{
	const size_t serializedSize =
		sizeof(aMessage.Severity) +
		sizeof(aMessage.Level) +
		sizeof(size_t) + aMessage.Message.length();

	std::vector<u8> data(serializedSize, 0);

	u8* ptr = data.data();

	memcpy(ptr, &aMessage.Severity, sizeof(aMessage.Severity));
	ptr += sizeof(aMessage.Severity);

	memcpy(ptr, &aMessage.Level, sizeof(aMessage.Level));
	ptr += sizeof(aMessage.Level);

	size_t size = aMessage.Message.length();
	memcpy(ptr, &size, sizeof(size));
	ptr += sizeof(size);

	memcpy(ptr, aMessage.Message.data(), aMessage.Message.length());
	ptr += aMessage.Message.length();

	return data;
}

Result<decltype(Failure), std::pair<LoggerRemoteMessageType, size_t>> LoggerRemoteMessageType::Deserialize(std::span<const u8> aData) noexcept
{
	constexpr size_t minSerializedSize =
		sizeof(Severity) +
		sizeof(Level) +
		sizeof(size_t);

	if (aData.size() < minSerializedSize)
		return Failure;

	LoggerRemoteMessageType message;

	size_t sizeMessage = 0;
	const u8* ptr = aData.data();

	memcpy(&message.Severity, ptr, sizeof(message.Severity));
	ptr += sizeof(message.Severity);

	memcpy(&message.Level, ptr, sizeof(message.Level));
	ptr += sizeof(message.Level);

	memcpy(&sizeMessage, ptr, sizeof(sizeMessage));
	ptr += sizeof(sizeMessage);

	if (aData.size() < minSerializedSize + sizeMessage)
		return Failure;

	message.Message.clear();

	if (sizeMessage == 0)
		return std::make_pair(message, minSerializedSize);

	message.Message.resize(sizeMessage, '\0');

	memcpy(message.Message.data(), ptr, sizeMessage);

	return std::make_pair(message, minSerializedSize + sizeMessage);
}
