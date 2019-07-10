#pragma once
#include <algorithm>

namespace MathUtility
{
	template<typename T>
	constexpr T Lerp(T aStart, T aEnd, const float aProgress)
	{
		return aStart + (aEnd - aStart) * aProgress;
	}
}