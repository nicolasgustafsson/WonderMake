#pragma once

#include "LoggerRemoteMessage.h"

#include "wondermake-utility/Endian.h"

std::vector<u8> SerializeLogline(const ProtoLoggerRemote::LogLine& aMessage) noexcept
{
	const u32 size = static_cast<u32>(aMessage.ByteSizeLong());

	std::vector<u8> serializedData(sizeof(size) + size, 0);

	*reinterpret_cast<u32*>(serializedData.data()) = ToEndianNetwork(size);

	aMessage.SerializeToArray(serializedData.data() + sizeof(size), static_cast<int>(serializedData.size()));

	return serializedData;
}

Result<std::pair<ProtoLoggerRemote::LogLine, size_t>> DeserializeLogline(std::span<const u8> aData) noexcept
{
	ProtoLoggerRemote::LogLine logline;

	const u32 size = ToEndianHost(*reinterpret_cast<const u32*>(aData.data()));

	if (aData.size() < sizeof(size) + size)
		return Err();

	const bool result = logline.ParseFromArray(aData.data() + sizeof(size), static_cast<int>(size));

	if (!result)
		return Err();

	const size_t readBytes = sizeof(size) + static_cast<size_t>(size);

	return Ok(std::make_pair(std::move(logline), readBytes));
}
