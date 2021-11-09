#pragma once

#include <compare>
#include <limits>
#include <numbers>
#include <ratio>
#include <type_traits>

#include "Utilities/WmMath.h"

#include "Typedefs.h"

template<typename TRep, typename TRatio>
class SRotation
{
public:
    using Representation = TRep;
    using Ratio = TRatio;

    inline [[nodiscard]] constexpr static SRotation Full() noexcept
    {
        return static_cast<TRep>(TRatio::num) / static_cast<TRep>(TRatio::den);
    }

    constexpr SRotation() = default;
    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation(const TFromRep aRotation) noexcept
        : Rotation(static_cast<TRep>(aRotation))
    {}
    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
        : SRotation(RotationCast<SRotation<TRep, TRatio>>(aRhs).Rotation)
    {}
    
    template<typename TRhsRep> requires WmMath::ComparableEquality<TRep, TRhsRep>
    inline [[nodiscard]] constexpr bool operator==(const TRhsRep aRhs) const noexcept
    {
        return Rotation == aRhs;
    }
    template<typename TRhsRep> requires WmMath::ComparableStrong<TRep, TRhsRep>
    inline [[nodiscard]] constexpr auto operator<=>(const TRhsRep& aRhs) const noexcept
    {
        return Rotation <=> aRhs;
    }

    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator=(const TFromRep aRhs) noexcept
    {
        Rotation = static_cast<TRep>(aRhs);

        return *this;
    }
    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator+=(const TFromRep aRhs) noexcept
    {
        Rotation += static_cast<TRep>(aRhs);

        return *this;
    }
    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator-=(const TFromRep aRhs) noexcept
    {
        Rotation -= static_cast<TRep>(aRhs);

        return *this;
    }
    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator*=(const TFromRep aRhs) noexcept
    {
        Rotation *= static_cast<TRep>(aRhs);

        return *this;
    }
    template<typename TFromRep> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator/=(const TFromRep aRhs) noexcept
    {
        Rotation /= static_cast<TRep>(aRhs);

        return *this;
    }
    
    template<typename TRhsRep, typename TRhsRatio> requires WmMath::ComparableEquality<TRep, TRhsRep>
    inline [[nodiscard]] constexpr bool operator==(const SRotation<TRhsRep, TRhsRatio> aRhs) const noexcept
    {
        return *this == RotationCast<SRotation<TRhsRep, TRatio>>(aRhs).Rotation;
    }
    template<typename TRhsRep, typename TRhsRatio> requires WmMath::ComparableStrong<TRep, TRhsRep>
    inline [[nodiscard]] constexpr auto operator<=>(const SRotation<TRhsRep, TRhsRatio>& aRhs) const noexcept
    {
        return *this <=> RotationCast<SRotation<TRhsRep, TRatio>>(aRhs).Rotation;
    }

    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator=(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
    {
        return *this = RotationCast<SRotation<TFromRep, TRatio>>(aRhs).Rotation;
    }
    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator+=(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
    {
        return *this += RotationCast<SRotation<TFromRep, TRatio>>(aRhs).Rotation;
    }
    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator-=(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
    {
        return *this -= RotationCast<SRotation<TFromRep, TRatio>>(aRhs).Rotation;
    }
    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator*=(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
    {
        return *this *= RotationCast<SRotation<TFromRep, TRatio>>(aRhs).Rotation;
    }
    template<typename TFromRep, typename TFromRatio> requires std::is_constructible_v<TRep, TFromRep>
    inline constexpr SRotation& operator/=(const SRotation<TFromRep, TFromRatio> aRhs) noexcept
    {
        return *this /= RotationCast<SRotation<TFromRep, TRatio>>(aRhs).Rotation;
    }

    TRep Rotation = {};
};

template<typename TRep, typename TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator+(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs += aRhs);
}
template<typename TRep, typename TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator-(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs -= aRhs);
}
template<typename TRep, typename TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator*(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs *= aRhs);
}
template<typename TRep, typename TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator/(SRotation<TRep, TRatio> aLhs, const TRep aRhs) noexcept
{
    return (aLhs /= aRhs);
}

template<typename TRep, typename TRatio, typename TFromRep, typename TFromRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator+(SRotation<TRep, TRatio> aLhs, const SRotation<TFromRep, TFromRatio> aRhs) noexcept
{
    return (aLhs += aRhs);
}
template<typename TRep, typename TRatio, typename TFromRep, typename TFromRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator-(SRotation<TRep, TRatio> aLhs, const SRotation<TFromRep, TFromRatio> aRhs) noexcept
{
    return (aLhs -= aRhs);
}
template<typename TRep, typename TRatio, typename TFromRep, typename TFromRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator*(SRotation<TRep, TRatio> aLhs, const SRotation<TFromRep, TFromRatio> aRhs) noexcept
{
    return (aLhs *= aRhs);
}
template<typename TRep, typename TRatio, typename TFromRep, typename TFromRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator/(SRotation<TRep, TRatio> aLhs, const SRotation<TFromRep, TFromRatio> aRhs) noexcept
{
    return (aLhs /= aRhs);
}

template <typename TToRotation, typename TFromRep, typename TFromRatio> requires std::_Is_specialization_v<TToRotation, SRotation>
inline [[nodiscard]] constexpr TToRotation RotationCast(const SRotation<TFromRep, TFromRatio> aRotation) noexcept
{
    using CommonFraction = std::ratio_divide<typename TToRotation::Ratio, TFromRatio>;
    using ToRep = typename TToRotation::Representation;
    using CommonRep = std::common_type_t<ToRep, TFromRep, f64>;

    constexpr auto num = CommonFraction::num;
    constexpr auto den = CommonFraction::den;

    constexpr bool numIsOne = num == 1;
    constexpr bool denIsOne = den == 1;

    if (denIsOne)
        if (numIsOne)
            return static_cast<TToRotation>(static_cast<ToRep>(aRotation.Rotation));
        else
            return static_cast<TToRotation>(static_cast<ToRep>(static_cast<CommonRep>(aRotation.Rotation) * static_cast<CommonRep>(num)));
    else
        if (numIsOne)
            return static_cast<TToRotation>(static_cast<ToRep>(static_cast<CommonRep>(aRotation.Rotation) / static_cast<CommonRep>(den)));
        else
            return static_cast<TToRotation>(static_cast<ToRep>(static_cast<CommonRep>(aRotation.Rotation) * static_cast<CommonRep>(num) / static_cast<CommonRep>(den)));
}

using DegreeRatio = std::ratio<360>;

template<typename TRep>
using SDegree = SRotation<TRep, DegreeRatio>;

using SDegreeF32 = SDegree<f32>;
using SDegreeF64 = SDegree<f64>;

constexpr static auto RepresentationPrecision = WmMath::Pow<intmax_t>(10, std::numeric_limits<intmax_t>::digits10);

template<typename TRep>
requires std::is_floating_point_v<TRep>
using RadianRatio = std::ratio<static_cast<intmax_t>(std::numbers::pi_v<TRep> * 2 * RepresentationPrecision), RepresentationPrecision>;

template<typename TRep>
using SRadian = SRotation<TRep, RadianRatio<TRep>>;

using SRadianF32 = SRadian<f32>;
using SRadianF64 = SRadian<f64>;

namespace WmMath
{
    template<typename TRep, typename TRatio, typename TReturnType = TRep> requires WmMath::has_floating_representation_v<TReturnType>
	inline [[nodiscard]] typename TRep Atan(const SRotation<TRep, TRatio> aRotation) noexcept
	{
		return WmMath::Atan(RotationCast<SRadian<TReturnType>>(aRotation).Rotation);
	}
	template<typename TRotation, typename TFromRep> requires WmMath::has_floating_representation_v<TFromRep>
	inline [[nodiscard]] TRotation Atan2(const TFromRep aY, const TFromRep aX) noexcept
	{
		return RotationCast<TRotation>(SRadian<typename TRotation::Representation>(Atan2(aY, aX)));
	}

	template<typename TRep, typename TRatio, typename TReturnType = TRep> requires WmMath::has_floating_representation_v<TReturnType>
	inline [[nodiscard]] typename TReturnType Cos(const SRotation<TRep, TRatio> aRotation) noexcept
	{
		return WmMath::Cos(RotationCast<SRadian<TReturnType>>(aRotation).Rotation);
	}
    template<typename TRep, typename TRatio, typename TReturnType = TRep> requires WmMath::has_floating_representation_v<TReturnType>
	inline [[nodiscard]] typename TReturnType Sin(const SRotation<TRep, TRatio> aRotation) noexcept
	{
		return WmMath::Sin(RotationCast<SRadian<TReturnType>>(aRotation).Rotation);
	}

    template<typename TRotation, typename TReturnType = typename TRotation::Representation>
    inline [[nodiscard]] TReturnType Mod(const TRotation aDividend, const TRotation aDivisor) noexcept
    {
        return WmMath::Mod(static_cast<TReturnType>(aDividend.Rotation), static_cast<TReturnType>(aDivisor.Rotation));
    }
    template<typename TRotation, typename TReturnType = typename TRotation::Representation>
    inline [[nodiscard]] TReturnType Remainder(const TRotation aDividend, const TRotation aDivisor) noexcept
    {
        return WmMath::Remainder(static_cast<TReturnType>(aDividend.Rotation), static_cast<TReturnType>(aDivisor.Rotation));
    }
}
