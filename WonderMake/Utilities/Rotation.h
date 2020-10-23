#pragma once

#include <compare>
#include <limits>
#include <numbers>
#include <ratio>
#include <type_traits>

#include "Utilities/MathUtility.h"

#include "Typedefs.h"

template<class TRep, class TRatio>
class SRotation
{
public:
    using Representation = TRep;
    using Ratio = TRatio;

    constexpr SRotation() = default;
    inline constexpr SRotation(const TRep aRotation) noexcept
        : myRotation(aRotation)
    {}

    inline [[nodiscard]] constexpr bool operator==(const TRep aRhs) const noexcept
    {
        return myRotation == aRhs;
    }
    inline [[nodiscard]] constexpr std::partial_ordering operator<=>(const TRep aRhs) const noexcept
    {
        return myRotation <=> aRhs;
    }

    template<class TFromRepresentation, class TFromRatio>
    inline [[nodiscard]] constexpr bool operator==(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
    {
        return *this == RotationCast<SRotation<TRep, TRatio>>(aRhs).myRotation;
    }
    template<class TFromRepresentation, class TFromRatio>
    inline [[nodiscard]] constexpr std::partial_ordering operator<=>(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
    {
        return *this <=> RotationCast<SRotation<TRep, TRatio>>(aRhs).myRotation;
    }

    inline constexpr SRotation& operator=(const TRep aRhs) noexcept
    {
        myRotation = aRhs;
        return *this;
    }
    inline constexpr SRotation& operator+=(const TRep aRhs) noexcept
    {
        myRotation += aRhs;
        return *this;
    }
    inline constexpr SRotation& operator-=(const TRep aRhs) noexcept
    {
        myRotation -= aRhs;
        return *this;
    }
    inline constexpr SRotation& operator*=(const TRep aRhs) noexcept
    {
        myRotation *= aRhs;
        return *this;
    }
    inline constexpr SRotation& operator/=(const TRep aRhs) noexcept
    {
        myRotation /= aRhs;
        return *this;
    }

    inline constexpr SRotation& operator=(const SRotation aRhs) noexcept
    {
        myRotation = aRhs.myRotation;
        return *this;
    }
    inline constexpr SRotation& operator+=(const SRotation aRhs) noexcept
    {
        myRotation += aRhs.myRotation;
        return *this;
    }
    inline constexpr SRotation& operator-=(const SRotation aRhs) noexcept
    {
        myRotation -= aRhs.myRotation;
        return *this;
    }
    inline constexpr SRotation& operator*=(const SRotation aRhs) noexcept
    {
        myRotation *= aRhs.myRotation;
        return *this;
    }
    inline constexpr SRotation& operator/=(const SRotation aRhs) noexcept
    {
        myRotation /= aRhs.myRotation;
        return *this;
    }

    inline [[nodiscard]] constexpr TRep Rotation() const noexcept
    {
        return myRotation;
    }

private:
    TRep myRotation = {};
};

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator+(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs += aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator-(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs -= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator*(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs *= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator/(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs /= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator+(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs += aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator-(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs -= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator*(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs *= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator/(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs /= aRhs);
}

template <class TToRotation, class FromTRepresentation, class TFromRatio>
requires std::_Is_specialization_v<TToRotation, SRotation>
[[nodiscard]] constexpr TToRotation RotationCast(const SRotation<FromTRepresentation, TFromRatio> aRotation) noexcept
{
    using _CF = std::ratio_divide<typename TToRotation::Ratio, TFromRatio>;

    using ToRepresentation = typename TToRotation::Representation;
    using CommonRepresentation = std::common_type_t<ToRepresentation, FromTRepresentation, f64>;

    constexpr auto num = _CF::num;
    constexpr auto den = _CF::den;

    constexpr bool numIsOne = num == 1;
    constexpr bool denIsOne = den == 1;

    if (denIsOne)
    {
        if (numIsOne)
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(aRotation.Rotation()));
        }
        else
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) * static_cast<CommonRepresentation>(num)));
        }
    }
    else
    {
        if (numIsOne)
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) / static_cast<CommonRepresentation>(den)));
        }
        else
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) * static_cast<CommonRepresentation>(num) / static_cast<CommonRepresentation>(den)));
        }
    }
}

using DegreeRatio = std::ratio<360>;

template<class TRep>
using SDegree = SRotation<TRep, DegreeRatio>;

using SDegreeF32 = SDegree<f32>;
using SDegreeF64 = SDegree<f64>;

constexpr static auto RepresentationPrecision = MathUtility::Pow<intmax_t>(10, std::numeric_limits<intmax_t>::digits10);

template<class TRep>
requires std::is_floating_point_v<TRep>
using RadianRatio = std::ratio<static_cast<intmax_t>(std::numbers::pi_v<TRep> * 2 * RepresentationPrecision), RepresentationPrecision>;

template<class TRep>
using SRadian = SRotation<TRep, RadianRatio<TRep>>;

using SRadianF32 = SRadian<f32>;
using SRadianF64 = SRadian<f64>;

namespace MathUtility
{
	template<typename TRotation>
    requires std::is_floating_point_v<typename TRotation::Representation>
	inline [[nodiscard]] typename TRotation::Representation Atan(const TRotation aRotation) noexcept
	{
		return MathUtility::Atan(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}

	template<typename TRotation>
    requires std::is_floating_point_v<typename TRotation::Representation>
	inline [[nodiscard]] TRotation Atan2(const typename TRotation::Representation aY, const typename TRotation::Representation aX) noexcept
	{
		return RotationCast<TRotation>(SRadian<typename TRotation::Representation>(Atan2(aY, aX)));
	}

	template<typename TRotation>
    requires std::is_floating_point_v<typename TRotation::Representation>
	inline [[nodiscard]] typename TRotation::Representation Cos(const TRotation aRotation) noexcept
	{
		return MathUtility::Cos(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}

	template<typename TRotation>
    requires std::is_floating_point_v<typename TRotation::Representation>
	inline [[nodiscard]] typename TRotation::Representation Sin(const TRotation aRotation) noexcept
	{
		return MathUtility::Sin(RotationCast<SRadian<typename TRotation::Representation>>(aRotation).Rotation());
	}
}
