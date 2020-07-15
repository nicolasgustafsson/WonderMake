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

	inline [[nodiscard]] f32 Atan(const f32 aRotation) noexcept
	{
		return std::atanf(aRotation);
	}

	inline [[nodiscard]] f64 Atan(const f64 aRotation) noexcept
	{
		return std::atan(aRotation);
	}

	inline [[nodiscard]] f32 Atan2(const f32 aY, const f32 aX) noexcept
	{
		return std::atan2f(aY, aX);
	}

	inline [[nodiscard]] f64 Atan2(const f64 aY, const f64 aX) noexcept
	{
		return std::atan2(aY, aX);
	}

	inline [[nodiscard]] f32 Cos(const f32 aRotation) noexcept
	{
		return std::cosf(aRotation);
	}

	inline [[nodiscard]] f64 Cos(const f64 aRotation) noexcept
	{
		return std::cos(aRotation);
	}

	inline [[nodiscard]] f32 Sin(const f32 aRotation) noexcept
	{
		return std::sinf(aRotation);
	}

	inline [[nodiscard]] f64 Sin(const f64 aRotation) noexcept
	{
		return std::sin(aRotation);
	}
}