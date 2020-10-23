#pragma once
#include "Typedefs.h"

#include "Utilities/Rotation.h"

#include <array>

template<typename TRep, u32 TSize>
struct SVectorBase;

#pragma warning(push)
#pragma warning(disable:4201)
template <typename TRep>
struct SVectorBase<TRep, 2>
{
public:
	union
	{
		struct
		{
			TRep X;
			TRep Y;
		};
		std::array<TRep, 2> MemberArray = {};
	};
};

template <typename TRep>
struct SVectorBase<TRep, 3>
{
public:
	union
	{
		struct
		{
			TRep X;
			TRep Y;
			TRep Z;
		};
		std::array<TRep, 3> MemberArray = {};
	};
};

template <typename TRep>
struct SVectorBase<TRep, 4>
{
public:
	union
	{
		struct
		{
			TRep X;
			TRep Y;
			TRep Z;
			TRep W;
		};
		std::array<TRep, 4> MemberArray = {};
	};
};
#pragma warning(pop)

template <typename TRep, u32 TSize>
requires (TSize > 4)
struct SVectorBase<TRep, TSize>
{
public:
	std::array<TRep, TSize> MemberArray = {};
};

template <typename TRep, u32 TSize>
struct SVector 
	: public SVectorBase<TRep, TSize>
{
	using Representation = TRep;
	static constexpr auto Size = TSize;

	constexpr SVector() = default;

	template<typename ...TArgs>
	constexpr SVector(TArgs... aArgs) noexcept
		requires (sizeof...(TArgs) == TSize && std::conjunction_v<std::is_convertible<TArgs, TRep>...>)
	{
		u32 i = 0;

		(((*this)[i++] = static_cast<TRep>(aArgs)), ...);
	}
	
	template<typename TRotation>
	SVector<TRep, TSize>& Rotate(const TRotation aRotation) noexcept
		requires (TSize == 2 && std::is_floating_point_v<typename TRotation::Representation>)
	{
		using RotationRep = SRadian<typename TRotation::Representation>;

		const auto rotation = RotationCast<RotationRep>(GetRotation<TRotation>() + aRotation);
		const auto length = Length();

		SVectorBase<TRep, Size>::X = MathUtility::Sin(rotation.Rotation());
		SVectorBase<TRep, Size>::Y = MathUtility::Cos(rotation.Rotation());

		SVectorBase<TRep, Size>::X *= length;
		SVectorBase<TRep, Size>::Y *= length;

		return (*this);
	}
	template<typename TRotation = SRadian<TRep>>
	[[nodiscard]] TRotation GetRotation() const noexcept
		requires (TSize == 2 && std::is_floating_point_v<typename TRotation::Representation>)
	{
		return MathUtility::Atan2<TRotation>(SVectorBase<TRep, Size>::X, SVectorBase<TRep, Size>::Y);
	}

	// Lowers or raises the dimension of the vector by one
	[[nodiscard]] constexpr TRep Demote() const noexcept
		requires (TSize == 2)
	{
		return (*this)[0];
	}
	[[nodiscard]] constexpr SVector<TRep, TSize - 1> Demote() const noexcept
		requires (TSize > 2)
	{
		SVector<TRep, Size - 1> ReturnVal;

		for (u32 u = 0; u < Size - 1; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}
	[[nodiscard]] constexpr SVector<TRep, TSize + 1> Promote(const TRep LastValue = {}) const noexcept
	{
		SVector<TRep, TSize + 1> ReturnVal;

		for (u32 u = 0; u < TSize; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		ReturnVal[TSize] = LastValue;

		return ReturnVal;
	}

	[[nodiscard]] constexpr TRep& operator[] (const u32 Index) noexcept
	{
		assert(Index < TSize);
		return this->MemberArray[Index];
	}
	[[nodiscard]] constexpr const TRep& operator[] (const u32 Index) const noexcept
	{
		assert(Index < TSize);
		return this->MemberArray[Index];
	}

	[[nodiscard]] constexpr char GetMemberName(const u32 Index) const noexcept
	{
		switch (Index)
		{
		case 0:
			return 'X';
		case 1:
			return 'Y';
		case 2:
			return 'Z';
		case 3:
			return 'W';
		default:
			return '1' + char(Index);
		}
	}

	[[nodiscard]] constexpr SVector<TRep, TSize> operator-() const noexcept
	{
		SVector<TRep, TSize> ReturnVal;

		for (u32 u = 0; u < TSize; u++)
		{
			ReturnVal[u] = -((*this)[u]);
		}

		return ReturnVal;
	}

	constexpr SVector<TRep, TSize>& operator*=(const TRep aRhs) noexcept
	{
		*this = *this * aRhs;

		return *this;
	}
	constexpr SVector<TRep, TSize>& operator/=(const TRep aRhs) noexcept
	{
		*this = *this / aRhs;

		return *this;
	}
	constexpr SVector<TRep, TSize>& operator+=(const SVector<TRep, TSize>& aRhs) noexcept
	{
		*this = *this + aRhs;

		return *this;
	}
	constexpr SVector<TRep, TSize>& operator-=(const SVector<TRep, TSize>& aRhs) noexcept
	{
		*this = *this - aRhs;

		return *this;
	}

	constexpr bool operator==(const SVector<TRep, TSize>& aRhs) const noexcept
	{
		for (u32 u = 0; u < TSize; u++)
		{
			if ((*this)[u] != aRhs[u])
				return false;
		}

		return true;
	}
	constexpr bool operator!=(const SVector<TRep, TSize>& aRhs) const noexcept
	{
		return !(*this == aRhs);
	}

	[[nodiscard]] constexpr TRep LengthSquared() const noexcept
	{
		TRep total = 0;

		for (u32 u = 0; u < TSize; u++)
		{
			total += (*this)[u] * (*this)[u];
		}

		return total;
	}
	[[nodiscard]] constexpr TRep Length() const noexcept
	{
		return static_cast<TRep>(std::sqrt(LengthSquared()));
	}
	[[nodiscard]] constexpr TRep DistanceTo(const SVector<TRep, TSize> aRhs) const noexcept
	{
		return (aRhs - *this).Length();
	}

	[[nodiscard]] constexpr TRep Dot(const SVector<TRep, TSize> aRhs) const noexcept
	{
		TRep sum = 0;
		for (u32 i = 0; i < TSize; i++)
		{
			sum += (*this)[i] * aRhs[i];
		}

		return sum;
	}

	constexpr void Normalize() noexcept
	{
		const TRep length = Length();

		if (length == 0)
			return;

		for (u32 u = 0; u < TSize; u++)
		{
			(*this)[u] /= length;
		}
	}
	[[nodiscard]] constexpr SVector<TRep, TSize> GetNormalized() const noexcept
	{
		SVector<TRep, TSize> retVec = *this;

		retVec.Normalize();

		return retVec;
	}
	
	[[nodiscard]] constexpr SVector<TRep, TSize> GetPerpendicularCounterClockWise() const noexcept
		requires (TSize == 2)
	{
		return SVector<TRep, TSize>(-(*this).Y, (*this).X);
	}
	[[nodiscard]] constexpr SVector<TRep, TSize> GetPerpendicularClockWise() const noexcept
		requires (TSize == 2)
	{
		return SVector<TRep, TSize>((*this).Y, -(*this).X);
	}
	[[nodiscard]] constexpr SVector<TRep, TSize> GetNormal() const noexcept
		requires (TSize == 2)
	{
		return GetPerpendicularClockWise();
	}

	[[nodiscard]] constexpr static SVector<TRep, TSize> Zero() noexcept
	{
		SVector<TRep, TSize> retVec;

		return retVec;
	}
	[[nodiscard]] constexpr static SVector<TRep, TSize> One() noexcept
	{
		SVector<TRep, TSize> retVec;

		for (u32 u = 0; u < TSize; u++)
		{
			retVec[u] = 1;
		}

		return retVec;
	}
};

template <typename TRep>
using SVector2 = SVector<TRep, 2>;
template <typename TRep>
using SVector3 = SVector<TRep, 3>;
template <typename TRep>
using SVector4 = SVector<TRep, 4>;

using SVector2f = SVector2<f32>;
using SVector3f = SVector3<f32>;
using SVector4f = SVector4<f32>;

using SVector2i = SVector2<i32>;
using SVector3i = SVector3<i32>;
using SVector4i = SVector4<i32>;

using SVector2u = SVector2<u32>;
using SVector3u = SVector3<u32>;
using SVector4u = SVector4<u32>;

template<typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator-(const SVector<TRep, TSize>& aLhs, const SVector<TRep, TSize>& aRhs) noexcept
{
	SVector<TRep, TSize> retVec = aLhs;

	for (u32 u = 0; u < TSize; u++)
	{
		retVec[u] -= aRhs[u];
	}

	return retVec;
}
template<typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator+(const SVector<TRep, TSize>& aLhs, const SVector<TRep, TSize>& aRhs) noexcept
{
	SVector<TRep, TSize> retVec = aLhs;

	for (u32 u = 0; u < TSize; u++)
	{
		retVec[u] += aRhs[u];
	}

	return retVec;
}
template<typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const TRep aLhs, const SVector<TRep, TSize>& aRhs) noexcept
{
	return aRhs * aLhs;
}
template<typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const SVector<TRep, TSize>& aLhs, const TRep aRhs) noexcept
{
	SVector<TRep, TSize> retVec = aLhs;

	for (u32 u = 0; u < TSize; u++)
	{
		retVec[u] *= aRhs;
	}

	return retVec;
}
template<typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator/(const SVector<TRep, TSize>& aLhs, const TRep aRhs) noexcept
{
	SVector<TRep, TSize> retVec = aLhs;

	for (u32 u = 0; u < TSize; u++)
	{
		retVec[u] /= aRhs;
	}

	return retVec;
}
