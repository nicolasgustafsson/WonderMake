#pragma once

#include "wondermake-utility/Typedefs.h"

#include <bit>
#include <type_traits>

inline constexpr bool IsEndianBig		= std::endian::native == std::endian::big;
inline constexpr bool IsEndianLittle	= std::endian::native == std::endian::little;

static_assert(IsEndianBig || IsEndianLittle, "Unknown-endianness detected.");

template<typename TUnsingedInteger>
inline constexpr [[nodiscard]] TUnsingedInteger SwapByteOrder(TUnsingedInteger aVal) noexcept requires(std::is_unsigned_v<TUnsingedInteger>)
{
	if constexpr (IsEndianBig)
		return aVal;
	else
	{
		TUnsingedInteger retVal = 0;

		for (size_t i = 0; i < sizeof(TUnsingedInteger) / 2; ++i)
			retVal |= ((aVal & (0xFF << (i * 8))) << ((sizeof(TUnsingedInteger) - 1 - (i * 2)) * 8));
		for (size_t i = 0; i < sizeof(TUnsingedInteger) / 2; ++i)
			retVal |= ((aVal & (0xFF << ((sizeof(TUnsingedInteger) - 1 - i) * 8))) >> ((sizeof(TUnsingedInteger) - 1 - (i * 2)) * 8));

		return retVal;
	}
}

template<typename TUnsingedInteger>
inline constexpr [[nodiscard]] TUnsingedInteger ToEndianNetwork(TUnsingedInteger aVal) noexcept requires(std::is_unsigned_v<TUnsingedInteger>)
{
	return SwapByteOrder(aVal);
}

template<typename TUnsingedInteger>
inline constexpr [[nodiscard]] TUnsingedInteger ToEndianHost(TUnsingedInteger aVal) noexcept requires(std::is_unsigned_v<TUnsingedInteger>)
{
	return SwapByteOrder(aVal);
}

// Quick bit of intrusive testing.
static_assert(!IsEndianBig		|| (SwapByteOrder<u32>(0x12345678) == 0x12345678));
static_assert(!IsEndianLittle	|| (SwapByteOrder<u32>(0x12345678) == 0x78563412));
