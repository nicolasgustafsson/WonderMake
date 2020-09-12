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

template <typename TRep, u32 TSize> requires (TSize > 4)
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

	template<typename... Reps>
	using TVectorMembers = typename std::enable_if_t<sizeof...(Reps) == TSize && std::conjunction_v<std::is_convertible<Reps, TRep>...>>;

	template<typename ...TArgs, typename = TVectorMembers<TArgs...>>
	constexpr SVector(TArgs... aArgs) noexcept;
	
	SVector<TRep, TSize>& Rotate(const SRadianF32 aRotation) noexcept requires (TSize == 2)
	{
		const auto rotation = GetRotation() + aRotation;
		const auto length = Length();

		SVectorBase<TRep, Size>::X = std::sinf(rotation.Rotation());
		SVectorBase<TRep, Size>::Y = std::cosf(rotation.Rotation());

		SVectorBase<TRep, Size>::X *= length;
		SVectorBase<TRep, Size>::Y *= length;

		return (*this);
	}
	[[nodiscard]] SRadianF32 GetRotation() const noexcept requires (TSize == 2)
	{
		return std::atan2f(SVectorBase<TRep, Size>::X, SVectorBase<TRep, Size>::Y);
	}

	// Lowers or raises the dimension of the vector by one
	[[nodiscard]] constexpr TRep Demote() const noexcept requires (TSize == 2)
	{
		return (*this)[0];
	}
	[[nodiscard]] constexpr SVector<TRep, TSize - 1> Demote() const noexcept requires (TSize > 2)
	{
		SVector<TRep, Size - 1> ReturnVal;

		for (u32 u = 0; u < Size - 1; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}
	[[nodiscard]] constexpr SVector<TRep, TSize + 1> Promote(const TRep LastValue = {}) const noexcept;

	[[nodiscard]] constexpr TRep& operator[] (const u32 Index) noexcept;
	[[nodiscard]] constexpr const TRep& operator[] (const u32 Index) const noexcept;

	[[nodiscard]] constexpr char GetMemberName(const u32 Index) const noexcept;

	[[nodiscard]] constexpr SVector<TRep, TSize> operator-() const noexcept;

	constexpr SVector<TRep, TSize>& operator*=(const TRep aMultiplier) noexcept;
	constexpr SVector<TRep, TSize>& operator/=(const TRep aDivisor) noexcept;
	constexpr SVector<TRep, TSize>& operator+=(const SVector<TRep, TSize>& aRightVector) noexcept;
	constexpr SVector<TRep, TSize>& operator-=(const SVector<TRep, TSize>& aRightVector) noexcept;

	constexpr bool operator==(const SVector<TRep, TSize>& aRightVector) const noexcept;
	constexpr bool operator!=(const SVector<TRep, TSize>& aRightVector) const noexcept;

	[[nodiscard]] constexpr TRep LengthSquared() const noexcept;
	[[nodiscard]] constexpr TRep Length() const noexcept;
	[[nodiscard]] constexpr TRep DistanceTo(const SVector<TRep, TSize> aOther) const noexcept;

	[[nodiscard]] constexpr TRep Dot(const SVector<TRep, TSize> aOther) const noexcept;

	constexpr void Normalize() noexcept;
	[[nodiscard]] constexpr SVector<TRep, TSize> GetNormalized() const noexcept;
	
	[[nodiscard]] constexpr SVector<TRep, TSize> GetPerpendicularCounterClockWise() const noexcept requires (TSize == 2)
	{
		return SVector<TRep, TSize>(-(*this).Y, (*this).X);
	}
	[[nodiscard]] constexpr SVector<TRep, TSize> GetPerpendicularClockWise() const noexcept requires (TSize == 2)
	{
		return SVector<TRep, TSize>((*this).Y, -(*this).X);
	}
	[[nodiscard]] constexpr SVector<TRep, TSize> GetNormal() const noexcept requires (TSize == 2)
	{
		return GetPerpendicularClockWise();
	}

	[[nodiscard]] constexpr static SVector<TRep, TSize> Zero() noexcept;
	[[nodiscard]] constexpr static SVector<TRep, TSize> One() noexcept;
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

template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator-(const SVector<TRep, TSize>& aLeftVector, const SVector<TRep, TSize>& aRightVector) noexcept;
template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator+(const SVector<TRep, TSize>& aLeftVector, const SVector<TRep, TSize>& aRightVector) noexcept;
template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const TRep aMultiplier, const SVector<TRep, TSize>& aLeftVector) noexcept;
template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator*(const SVector<TRep, TSize>& aLeftVector, const TRep aMultiplier) noexcept;
template <typename TRep, u32 TSize>
[[nodiscard]] constexpr static SVector<TRep, TSize> operator/(const SVector<TRep, TSize>& aLeftVector, const TRep aDivisor) noexcept;

#include "Vector.inl"
