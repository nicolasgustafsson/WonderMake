#pragma once

namespace MathUtility
{
	template<typename T>
	constexpr T Lerp(T aStart, T aEnd, const float aProgress)
	{
		return static_cast<T>(aStart + (aEnd - aStart) * aProgress);
	}
}