#pragma once

namespace MathUtility
{
	template<typename T>
	[[nodiscard]] constexpr T Clamp(const T aMin, const T aMax, const T aValue) noexcept
	{
		return aValue < aMin ? aMin : aValue > aMax ? aMax : aValue;
	}

	template<typename T>
	constexpr T Lerp(T aStart, T aEnd, const float aProgress)
	{
		return static_cast<T>(aStart + (aEnd - aStart) * aProgress);
	}
}