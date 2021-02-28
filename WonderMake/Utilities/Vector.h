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

	template<typename TRotationRep,
		typename TRotation = SDegree<TRotationRep>>
		requires std::is_floating_point_v<TRotationRep>
	SVector<TRep, TSize>& RotateCounterClockwise(const TRotationRep aRotation) noexcept
		requires (TSize == 2)
	{
		return RotateCounterClockwise<TRotation>(aRotation);
	}
	template<typename TRotation = SDegree<TRep>>
		requires std::is_floating_point_v<typename TRotation::Representation>
	SVector<TRep, TSize>& RotateCounterClockwise(const TRotation aRotation) noexcept
		requires (TSize == 2)
	{
		const auto rotation = GetAngle<TRotation>(0) + aRotation;
		const auto length = Length();

		SVectorBase<TRep, Size>::X = static_cast<TRep>(WmMath::Cos(rotation) * length);
		SVectorBase<TRep, Size>::Y = static_cast<TRep>(WmMath::Sin(rotation) * length);

		return (*this);
	}
	template<typename TRotationRep,
		typename TRotation = SDegree<TRotationRep>>
		requires std::is_floating_point_v<TRotationRep>
	SVector<TRep, TSize>& RotateClockwise(const TRotationRep aRotation) noexcept
		requires (TSize == 2)
	{
		return RotateClockwise<TRotation>(aRotation);
	}
	template<typename TRotation = SDegree<TRep>>
		requires std::is_floating_point_v<typename TRotation::Representation>
	SVector<TRep, TSize>& RotateClockwise(const TRotation aRotation) noexcept
		requires (TSize == 2)
	{
		return RotateCounterClockwise(aRotation * -1.f);
	}
	
	template<typename TRotation,
		typename TReturnRotation = SDegree<TRotation>>
		requires std::is_floating_point_v<TRotation>
			&& std::is_floating_point_v<typename TReturnRotation::Representation>
	[[nodiscard]] TReturnRotation GetAngle(const TRotation aRotation) const noexcept
		requires (TSize == 2)
	{
		return GetAngle<TReturnRotation>(aRotation);
	}
	template<typename TRotation = SDegree<TRep>,
		typename TReturnRotation = TRotation>
		requires std::is_floating_point_v<typename TRotation::Representation>
			&& std::is_floating_point_v<typename TReturnRotation::Representation>
	[[nodiscard]] TReturnRotation GetAngle(const TRotation aRotation) const noexcept
		requires (TSize == 2)
	{
		return WmMath::Remainder(WmMath::Atan2<TRotation>(SVectorBase<TRep, Size>::Y, SVectorBase<TRep, Size>::X) - aRotation, TReturnRotation::Full());
	}
	template<typename TReturnRotation = SDegree<TRep>>
		requires std::is_floating_point_v<typename TReturnRotation::Representation>
	[[nodiscard]] TReturnRotation GetAngle(const SVector<TRep, TSize> aOtherVector) const noexcept
		requires (TSize == 2)
	{
		return GetAngle(aOtherVector.GetAngle<TReturnRotation>(0));
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

	template<u64 TArraySize, typename TVector> requires std::is_same_v<SVector, std::decay_t<TVector>>
	[[nodiscard]] static constexpr TVector& Closest(SVector aPoint, std::array<TVector*, TArraySize> aArray) noexcept
	{
		u64 index = 0;
		f32 distance = std::numeric_limits<f32>::max();

		for (u64 i = 0; i < aArray.size(); i++)
		{
			auto testVector = *aArray[i];
			const f32 newDistance = aPoint.DistanceTo(testVector);
			if (newDistance < distance)
			{
				distance = newDistance;
				index = i;
			}
		}
		
		return *(aArray[index]);
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

	constexpr void Floor() noexcept
	{
		for (u32 i = 0; i < TSize; i++)
		{
			(*this)[i] = std::floorf((*this)[i]);
		}
	}

	constexpr SVector<TRep, TSize> GetFloored() const noexcept
	{
		SVector<TRep, TSize> vec = *this;

		vec.Floor();

		return vec;
	}

	constexpr void Round() noexcept
	{
		for (u32 i = 0; i < TSize; i++)
		{
			(*this)[i] = std::roundf((*this)[i]);
		}
	}

	constexpr SVector<TRep, TSize> GetRounded() const noexcept
	{
		SVector<TRep, TSize> vec = *this;

		vec.Round();

		return vec;
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
