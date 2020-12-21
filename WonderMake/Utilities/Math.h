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

	template <class TLhs, class TRhs>
	concept ComparableEqual = requires (TLhs aLhs, TRhs aRhs) { aLhs == aRhs; };
	template <class TLhs, class TRhs>
	concept ComparableNEqual = requires (TLhs aLhs, TRhs aRhs) { aLhs != aRhs; };
	template <class TLhs, class TRhs>
	concept ComparableLess = requires (TLhs aLhs, TRhs aRhs) { aLhs < aRhs; };
	template <class TLhs, class TRhs>
	concept ComparableGreater = requires (TLhs aLhs, TRhs aRhs) { aLhs > aRhs; };
	template <class TLhs, class TRhs>
	concept ComparableEqualLess = requires (TLhs aLhs, TRhs aRhs) { aLhs <= aRhs; };
	template <class TLhs, class TRhs>
	concept ComparableEqualGreater = requires (TLhs aLhs, TRhs aRhs) { aLhs >= aRhs; };

	template <class TLhs, class TRhs>
	concept ComparableWeak = ComparableLess<TLhs, TRhs> && ComparableGreater<TLhs, TRhs>;
	template <class TLhs, class TRhs>
	concept ComparableStrong = ComparableWeak<TLhs, TRhs> && ComparableEqualLess<TLhs, TRhs> && ComparableEqualGreater<TLhs, TRhs>;
	template <class TLhs, class TRhs>
	concept ComparableEquality = ComparableEqual<TLhs, TRhs> && ComparableNEqual<TLhs, TRhs>;

	template <class TLhs, class TRhs>
	concept Comparable = ComparableEquality<TLhs, TRhs> && ComparableStrong<TLhs, TRhs>;

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