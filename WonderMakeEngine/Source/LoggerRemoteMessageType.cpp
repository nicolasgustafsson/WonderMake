#pragma once

#include "LoggerRemoteMessageType.h"

std::vector<u8> LoggerRemoteMessageType::Serialize() const noexcept
{
	const size_t serializedSize =
		sizeof(Severity) +
		sizeof(Level) +
		sizeof(size_t) + Message.length();

	std::vector<u8> data(serializedSize, 0);

	u8* ptr = data.data();

	memcpy(ptr, &Severity, sizeof(Severity));
	ptr += sizeof(Severity);

	memcpy(ptr, &Level, sizeof(Level));
	ptr += sizeof(Level);

	size_t size = Message.length();
	memcpy(ptr, &size, sizeof(size));
	ptr += sizeof(size);

	memcpy(ptr, Message.data(), Message.length());
	ptr += Message.length();

	return data;
}

size_t LoggerRemoteMessageType::Deserialize(const std::span<u8>& aData) noexcept
{
	constexpr size_t minSerializedSize =
		sizeof(Severity) +
		sizeof(Level) +
		sizeof(size_t);

	if (aData.size() < minSerializedSize)
		return 0;

	size_t sizeMessage = 0;
	const u8* ptr = aData.data();

	memcpy(&Severity, ptr, sizeof(Severity));
	ptr += sizeof(Severity);

	memcpy(&Level, ptr, sizeof(Level));
	ptr += sizeof(Level);

	memcpy(&sizeMessage, ptr, sizeof(sizeMessage));
	ptr += sizeof(sizeMessage);

	if (aData.size() < minSerializedSize + sizeMessage)
		return 0;

	Message.clear();

	if (sizeMessage == 0)
		return minSerializedSize;

	Message.resize(sizeMessage, '\0');

	memcpy(Message.data(), ptr, sizeMessage);

	return minSerializedSize + sizeMessage;
}
