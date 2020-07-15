#pragma once
#include <concepts>

namespace WmMath
{
	template <class T>
	concept Addable = requires (T x) { x + x; };
	template <class T>
	concept Subtractable = requires (T x) { x - x; };
	template <class T> //[Nicos]: TODO make y be any floating point number
	concept Progressable = requires (T x, f32 y) { x * y; };
	template <class T>
	concept Interpolable = Addable<T> && Subtractable<T> && Progressable<T>;


	template <class T> requires Interpolable<T>
	T Lerp(const T aStart, const T aEnd, const f32 aProgress)
	{
		return aStart + ((aEnd - aStart) * aProgress);
	}

	template <class T> requires std::totally_ordered<T>
		T Clamp(const T aMin, const T aMax, const T aValue)
		{
			if (aValue < aMin)
				return aMin;

			if (aValue > aMax)
				return aMax;

			return aValue;
		}
}