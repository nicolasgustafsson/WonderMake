#pragma once

#include "Typedefs.h"

namespace MathUtility
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

	template<typename T>
	using FloatingRepresentation = decltype(_FloatingRepresentation<T>());

	template<typename T>
	concept has_floating_representation_v = !std::is_same_v<FloatingRepresentation<T>, void>;

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