#pragma once

#include <compare>
#include <limits>
#include <numbers>
#include <ratio>
#include <type_traits>

#include "Utilities/MathUtility.h"

#include "Typedefs.h"

template<class TRepresentation, class TRatio>
class SRotation
{
public:
    using Representation = TRepresentation;
    using Ratio = TRatio;

    constexpr SRotation() = default;
    inline constexpr SRotation(const TRepresentation aRotation) noexcept;

    inline [[nodiscard]] constexpr bool operator==(const TRepresentation aRhs) const noexcept;
    inline [[nodiscard]] constexpr std::partial_ordering operator<=>(const TRepresentation aRhs) const noexcept;

    template<class TFromRepresentation, class TFromRatio>
    inline [[nodiscard]] constexpr bool operator==(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept;
    template<class TFromRepresentation, class TFromRatio>
    inline [[nodiscard]] constexpr std::partial_ordering operator<=>(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept;

    inline constexpr SRotation& operator=(const TRepresentation aRhs) noexcept;
    inline constexpr SRotation& operator+=(const TRepresentation aRhs) noexcept;
    inline constexpr SRotation& operator-=(const TRepresentation aRhs) noexcept;

    inline constexpr SRotation& operator=(const SRotation aRhs) noexcept;
    inline constexpr SRotation& operator+=(const SRotation aRhs) noexcept;
    inline constexpr SRotation& operator-=(const SRotation aRhs) noexcept;

    inline [[nodiscard]] constexpr TRepresentation Rotation() const noexcept;

private:
    TRepresentation myRotation = {};
};

template <class TToRotation, class FromTRepresentation, class TFromRatio>
requires std::_Is_specialization_v<TToRotation, SRotation>
[[nodiscard]] constexpr TToRotation RotationCast(const SRotation<FromTRepresentation, TFromRatio> aRotation) noexcept;

using DegreeRatio = std::ratio<360>;

template<class TRepresentation>
using SDegree = SRotation<TRepresentation, DegreeRatio>;

using SDegreeF32 = SDegree<f32>;
using SDegreeF64 = SDegree<f64>;

constexpr static auto RepresentationPrecision = MathUtility::Pow<intmax_t>(10, std::numeric_limits<intmax_t>::digits10);

template<class TRepresentation>
using RadianRatio = std::ratio<static_cast<intmax_t>(std::numbers::pi_v<TRepresentation> * 2 * RepresentationPrecision), RepresentationPrecision>;

using RadianF32 = RadianRatio<f32>;
using RadianF64 = RadianRatio<f64>;

template<class TRepresentation>
using SRadian = SRotation<TRepresentation, RadianRatio<TRepresentation>>;

using SRadianF32 = SRadian<f32>;
using SRadianF64 = SRadian<f64>;

namespace MathUtility
{
	template<typename TRotation>
	inline [[nodiscard]] typename TRotation::Representation Atan(const TRotation aRotation) noexcept
	{
		return MathUtility::Atan(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}

	template<typename TRotation>
	inline [[nodiscard]] TRotation Atan2(const typename TRotation::Representation aY, const typename TRotation::Representation aX) noexcept
	{
		return RotationCast<TRotation>(SRadian<typename TRotation::Representation>(Atan2(aY, aX)));
	}

	template<typename TRotation>
	inline [[nodiscard]] typename TRotation::Representation Cos(const TRotation aRotation) noexcept
	{
		return MathUtility::Cos(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}

	template<typename TRotation>
	inline [[nodiscard]] typename TRotation::Representation Sin(const TRotation aRotation) noexcept
	{
		return MathUtility::Sin(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}
}

#include "Rotation.inl"
