#pragma once
#include "Typedefs.h"

template<typename T, u32 Size, typename Enable = void>
struct SVectorBase;

#pragma warning(push)
#pragma warning(disable:4201)
template <typename T>
struct SVectorBase<T, 2>
{
public:
	union
	{
		struct
		{
			T X;
			T Y;
		};
		std::array<T, 2> MemberArray = {};
	};

};

template <typename T>
struct SVectorBase<T, 3>
{
public:
	union
	{
		struct
		{
			T X;
			T Y;
			T Z;
		};
		std::array<T, 3> MemberArray = {};
	};
};

template <typename T>
struct SVectorBase<T, 4>
{
public:
	union
	{
		struct
		{
			T X;
			T Y;
			T Z;
			T W;
		};
		std::array<T, 4> MemberArray = {};
	};
};
#pragma warning(pop)

template <typename T, u32 Size> 
struct SVectorBase<T, Size, std::enable_if_t<(Size > 4)>>
{
public:
	std::array<T, Size> MemberArray = {};
};

template <typename T, u32 Size>
struct SVector 
	: public SVectorBase<T, Size>
{
	//put generic vector stuff here
	constexpr SVector() = default;

	template<typename... Ts>
	using TVectorMembers = typename std::enable_if_t<sizeof...(Ts) == Size && std::conjunction_v<std::is_convertible<Ts, T>...>>;

	template<typename ...TArgs, typename = TVectorMembers<TArgs...>>
	constexpr SVector(TArgs... aArgs) noexcept
	{
		u32 i = 0;

		(((*this)[i++] = aArgs), ...);
	}

	//lowers the dimension of the vector by one
	template<class Q = T>
	[[nodiscard]] constexpr typename std::enable_if_t<(Size == 2), Q> Demote() const noexcept
	{
		return (*this)[0];
	}

	//lowers the dimension of the vector by one
	template<class Q = T>
	[[nodiscard]] constexpr typename std::enable_if_t<(Size > 2), SVector<Q, Size - 1>> Demote() const noexcept
	{
		SVector<Q, Size - 1> ReturnVal;

		for (u32 u = 0; u < Size - 1; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}

	//raises the dimension of the vector by one
	[[nodiscard]] constexpr SVector<T, Size + 1> Promote(const T LastValue = {}) const noexcept
	{
		SVector<T, Size + 1> ReturnVal;

		for (u32 u = 0; u < Size; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		ReturnVal[Size] = LastValue;

		return ReturnVal;
	}

	[[nodiscard]] constexpr T& operator[] (const u32 Index) noexcept
	{
		assert(Index < Size);
		return this->MemberArray[Index];
	}

	[[nodiscard]] constexpr const T& operator[] (const u32 Index) const noexcept
	{
		assert(Index < Size);
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
			return '1' + Index;
		}
	}

	[[nodiscard]] constexpr SVector<T, Size> operator-() const noexcept
	{
		SVector<T, Size> ReturnVal;

		for (u32 u = 0; u < Size; u++)
		{
			ReturnVal[u] = -((*this)[u]);
		}

		return ReturnVal;
	}


	constexpr SVector<T, Size>& operator*=(const T aMultiplier) noexcept
	{
		*this = *this * aMultiplier;

		return *this;
	}

	constexpr SVector<T, Size>& operator/=(const T aDivisor) noexcept
	{
		*this = *this / aDivisor;

		return *this;
	}

	constexpr SVector<T, Size>& operator+=(const SVector<T, Size>& aRightVector) noexcept
	{
		*this = *this + aRightVector;

		return *this;
	}

	constexpr SVector<T, Size>& operator-=(const SVector<T, Size>& aRightVector) noexcept
	{
		*this = *this - aRightVector;

		return *this;
	}

	[[nodiscard]] constexpr static SVector<T, Size> Zero() noexcept
	{
		SVector<T, Size> ReturnVal;

		return ReturnVal;
	}

	[[nodiscard]] constexpr static SVector<T, Size> One() noexcept
	{
		SVector<T, Size> ReturnVal;

		for (u32 u = 0; u < Size; u++)
		{
			ReturnVal[u] = 1;
		}

		return ReturnVal;
	}

	[[nodiscard]] constexpr T LengthSquared() const noexcept
	{
		T total = 0;

		for (u32 u = 0; u < Size; u++)
		{
			total += (*this)[u] * (*this)[u];
		}

		return total;
	}

	[[nodiscard]] constexpr T Length() const noexcept
	{
		return std::sqrt(LengthSquared());
	}

	constexpr void Normalize() noexcept
	{
		const T length = Length();

		if (length == 0)
			return;

		for (u32 u = 0; u < Size; u++)
		{
			(*this)[u] /= length;
		}
	}

	[[nodiscard]] constexpr SVector<T, Size> GetNormalized() const noexcept
	{
		SVector<T, Size> vec = *this;

		vec.Normalize();

		return vec;
	}
};

template <typename T>
using SVector2 = SVector<T, 2>;

template <typename T>
using SVector3 = SVector<T, 3>;

template <typename T>
using SVector4 = SVector<T, 4>;


using SVector2f = SVector2<f32>;
using SVector3f = SVector3<f32>;
using SVector4f = SVector4<f32>;

using SVector2i = SVector2<i32>;
using SVector3i = SVector3<i32>;
using SVector4i = SVector4<i32>;

using SVector2u = SVector2<u32>;
using SVector3u = SVector3<u32>;
using SVector4u = SVector4<u32>;

template <typename T, u32 Size>
[[nodiscard]] constexpr static SVector<T, Size> operator-(const SVector<T, Size>& aLeftVector, const SVector<T, Size>& aRightVector) noexcept
{
	SVector<T, Size> ReturnVal = aLeftVector;

	for (u32 u = 0; u < Size; u++)
	{
		ReturnVal[u] -= aRightVector[u];
	}

	return ReturnVal;
}

template <typename T, u32 Size>
[[nodiscard]] constexpr static SVector<T, Size> operator+(const SVector<T, Size>& aLeftVector, const SVector<T, Size>& aRightVector) noexcept
{
	SVector<T, Size> ReturnVal = aLeftVector;

	for (u32 u = 0; u < Size; u++)
	{
		ReturnVal[u] += aRightVector[u];
	}

	return ReturnVal;
}

template <typename T, u32 Size>
[[nodiscard]] constexpr static SVector<T, Size> operator*(const T aMultiplier, const SVector<T, Size>& aLeftVector) noexcept
{
	return aLeftVector * aMultiplier;
}

template <typename T, u32 Size>
[[nodiscard]] constexpr static SVector<T, Size> operator*(const SVector<T, Size>& aLeftVector, const T aMultiplier) noexcept
{
	SVector<T, Size> ReturnVal = aLeftVector;

	for (u32 u = 0; u < Size; u++)
	{
		ReturnVal[u] *= aMultiplier;
	}

	return ReturnVal;
}

template <typename T, u32 Size>
[[nodiscard]] constexpr static SVector<T, Size> operator/(const SVector<T, Size>& aLeftVector, const T aDivisor) noexcept
{
	SVector<T, Size> ReturnVal = aLeftVector;

	for (u32 u = 0; u < Size; u++)
	{
		ReturnVal[u] /= aDivisor;
	}

	return ReturnVal;
}
