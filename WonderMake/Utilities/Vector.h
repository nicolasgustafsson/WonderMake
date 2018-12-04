#pragma once
#include "Typedefs.h"

template<typename T, u32 Size, typename Enable = void>
struct SVectorBase;

template <typename T>
struct SVectorBase<T, 2>
{
public:
	T X = {};
	T Y = {};
};

template <typename T>
struct SVectorBase<T, 3>
{
public:
	T X = {};
	T Y = {};
	T Z = {};
};

template <typename T>
struct SVectorBase<T, 4>
{
public:
	T X = {};
	T Y = {};
	T Z = {};
	T W = {};
};

template <typename T, u32 Size>
struct SVectorBase<T, Size, std::enable_if_t<(Size > 4)>>
{
public:
	std::array<T, Size> MemberArray = {};
};

template <typename T, u32 Size>
struct SVector : SVectorBase<T, Size>
{
	//put generic vector stuff here
	constexpr SVector() = default;

	//lowers the dimension of the vector by one
	template<class Q = T>
	constexpr typename std::enable_if_t<(Size == 2), Q> Demote() const
	{
		return (*this)[0];
	}

	//lowers the dimension of the vector by one
	template<class Q = T>
	constexpr typename std::enable_if_t<(Size > 2), SVector<Q, Size - 1>> Demote() const
	{
		SVector<Q, Size - 1> ReturnVal;

		for (u32 u = 0; u < Size - 1; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}

	//raises the dimension of the vector by one
	constexpr SVector<T, Size + 1> Promote(const T LastValue = {}) const
	{
		SVector<T, Size + 1> ReturnVal;

		for (u32 u = 0; u < Size; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		ReturnVal[Size] = LastValue;

		return ReturnVal;
	}

	constexpr T& operator[] (const u32 Index)
	{
		T* PointerToMember = reinterpret_cast<T*>(this);
		PointerToMember += Index;
		return *PointerToMember;
	}

	constexpr const T& operator[] (const u32 Index) const
	{
		const T* PointerToMember = reinterpret_cast<const T*>(this);
		PointerToMember += Index;
		return *PointerToMember;
	}

	void Print()
	{
		for (u32 u = 0; u < Size; u++)
		{
			std::cout << GetMemberName(u) << ": " << (*this)[u] << "\n";
		}
	}

	constexpr char GetMemberName(const u32 Index) const
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