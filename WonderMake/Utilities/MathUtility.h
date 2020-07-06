#pragma once

namespace MathUtility
{
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
}