#pragma once

#include "Typedefs.h"

namespace MathUtility
{
	template<typename T>
	[[nodiscard]] constexpr T Clamp(const T aMin, const T aMax, const T aValue) noexcept
	{
		return aValue < aMin ? aMin : aValue > aMax ? aMax : aValue;
	}

	template<typename T>
	inline [[nodiscard]] constexpr T Lerp(T aStart, T aEnd, const float aProgress) noexcept
	{
		return static_cast<T>(aStart + (aEnd - aStart) * aProgress);
	}

	template<typename T>
	inline [[nodiscard]] constexpr T Pow(const T aBase, const uintmax_t aExponent) noexcept
	{
		return aExponent == 0 ? 1 : aBase * Pow(aBase, aExponent - 1);
	}

	template<typename T>
	inline [[nodiscard]] T Atan(const T aRotation) noexcept
		requires std::is_floating_point_v<T>
	{
		return std::atan(aRotation);
	}

	template<typename T>
	inline [[nodiscard]] T Atan2(const T aY, const T aX) noexcept
		requires std::is_floating_point_v<T>
	{
		return std::atan2(aY, aX);
	}

	template<typename T>
	inline [[nodiscard]] T Cos(const T aRotation) noexcept
		requires std::is_floating_point_v<T>
	{
		return std::cos(aRotation);
	}

	template<typename T>
	inline [[nodiscard]] T Sin(const T aRotation) noexcept
		requires std::is_floating_point_v<T>
	{
		return std::sin(aRotation);
	}
}