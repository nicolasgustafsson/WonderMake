#pragma once
#include <concepts>
#include <cmath>

#include "Typedefs.h"

namespace WmMath
{
	namespace _Impl
	{
		template<typename T>
		constexpr auto _FloatingRepresentation()
		{
			if constexpr (std::is_same_v<T, f32>
				|| std::is_same_v<T, u8>
				|| std::is_same_v<T, u16>
				|| std::is_same_v<T, u32>
				|| std::is_same_v<T, i8>
				|| std::is_same_v<T, i16>
				|| std::is_same_v<T, i32>)
				return f32();
			if constexpr (std::is_same_v<T, f64>
				|| std::is_same_v<T, u64>
				|| std::is_same_v<T, i64>)
				return f64();
		}
	}

	template<typename T>
	using FloatingRepresentation = decltype(_Impl::_FloatingRepresentation<T>());

	template<typename T>
	concept has_floating_representation_v = !std::is_same_v<FloatingRepresentation<T>, void>;

	template <class T>
	concept Addable = requires (T x) { x + x; };
	template <class T>
	concept Subtractable = requires (T x) { x - x; };
	template <class T>
	concept Multipliable = requires (T x) { x * x; };
	template <class T>
	concept Divisible = requires (T x) { x / x; };

	template <class T>
	concept Arithmetic =
		Addable<T> &&
		Subtractable<T> &&
		Multipliable<T> &&
		Divisible<T>;

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

	template <typename TLhs, typename TRhs>
		requires(ComparableLess<TLhs, TRhs>)
	inline [[nodiscard]] constexpr TLhs Min(TLhs&& aLhs, TRhs&& aRhs)
		noexcept(std::is_nothrow_move_constructible_v<TLhs> && std::is_nothrow_constructible_v<TLhs, decltype(std::forward<TRhs>(aRhs))>)
	{
		return aLhs < aRhs ? std::forward<TLhs>(aLhs) : std::forward<TRhs>(aRhs);
	}
	template <typename TLhs, typename TRhs>
		requires(ComparableGreater<TLhs, TRhs>)
	inline [[nodiscard]] constexpr TLhs Max(TLhs&& aLhs, TRhs&& aRhs)
		noexcept(std::is_nothrow_move_constructible_v<TLhs>&& std::is_nothrow_constructible_v<TLhs, decltype(std::forward<TRhs>(aRhs))>)
	{
		return aLhs > aRhs ? std::forward<TLhs>(aLhs) : std::forward<TRhs>(aRhs);
	}

	template <Interpolable T>
	inline [[nodiscard]] constexpr T Lerp(const T aStart, const T aEnd, const f32 aProgress) noexcept
	{
		return aStart + ((aEnd - aStart) * aProgress);
	}

	template <class T> requires std::totally_ordered<T>
	inline [[nodiscard]] constexpr T Clamp(const T aMin, const T aMax, const T aValue) noexcept
	{
		if (aValue < aMin)
			return aMin;

		if (aValue > aMax)
			return aMax;

		return aValue;
	}

	template<typename T>
	inline [[nodiscard]] constexpr T Pow(const T aBase, const uintmax_t aExponent) noexcept
	{
		return aExponent == 0 ? 1 : aBase * Pow(aBase, aExponent - 1);
	}

	template<typename T>
	inline [[nodiscard]] constexpr T Square(const T aBase) noexcept
	{
		return Pow(aBase, 2);
	}

	template<typename T>
	inline [[nodiscard]] constexpr T Cube(const T aBase) noexcept
	{
		return Pow(aBase, 3);
	}

	template<typename T>
	requires has_floating_representation_v<T>
		inline [[nodiscard]] FloatingRepresentation<T> Atan(const T aRotation) noexcept
	{
		return std::atan(static_cast<FloatingRepresentation<T>>(aRotation));
	}

	template<typename T>
	requires has_floating_representation_v<T>
		inline [[nodiscard]] FloatingRepresentation<T> Atan2(const T aY, const T aX) noexcept
	{
		return std::atan2(static_cast<FloatingRepresentation<T>>(aY), static_cast<FloatingRepresentation<T>>(aX));
	}

	template<typename T>
	requires has_floating_representation_v<T>
		inline [[nodiscard]] FloatingRepresentation<T> Cos(const T aRotation) noexcept
	{
		return std::cos(static_cast<FloatingRepresentation<T>>(aRotation));
	}

	template<typename T>
	requires has_floating_representation_v<T>
		inline [[nodiscard]] FloatingRepresentation<T> Sin(const T aRotation) noexcept
	{
		return std::sin(static_cast<FloatingRepresentation<T>>(aRotation));
	}

	template<typename T>
	requires std::is_floating_point_v<T>
		inline [[nodiscard]] T Mod(const T aDividend, const T aDivisor) noexcept
	{
		return std::fmod(aDividend, aDivisor);
	}

	template<typename T>
	requires std::is_integral_v<T>
		inline [[nodiscard]] T Mod(const T aDividend, const T aDivisor) noexcept
	{
		return aDividend % aDivisor;
	}

	template<typename T>
	requires std::is_floating_point_v<T>
		inline [[nodiscard]] T Remainder(const T aDividend, const T aDivisor) noexcept
	{
		return std::remainder(aDividend, aDivisor);
	}
}