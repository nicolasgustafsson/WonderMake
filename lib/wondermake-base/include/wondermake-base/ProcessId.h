#pragma once

#include "wondermake-utility/Guid.h"
#include "wondermake-utility/Typedefs.h"

class ProcessId
{
public:
	ProcessId() = default;
	inline constexpr ProcessId(const u32 aId) noexcept
	{
		myId[0] = static_cast<u8>(0xFF & aId);
		myId[1] = static_cast<u8>(0xFF & (aId >> 8));
		myId[2] = static_cast<u8>(0xFF & (aId >> 16));
		myId[3] = static_cast<u8>(0xFF & (aId >> 24));
	}

	[[nodiscard]] inline constexpr bool operator==(const ProcessId&) const noexcept = default;

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		return myId.Hash();
	}

	[[nodiscard]] inline constexpr Guid Id() const noexcept
	{
		return myId;
	}

private:
	Guid myId = Guid::Zero();

};

namespace std
{
	template <>
	struct hash<ProcessId>
	{
		[[nodiscard]] inline size_t operator()(const ProcessId aId) const noexcept
		{
			return aId.Hash();
		}
	};
}
