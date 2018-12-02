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
struct SVector : SVectorBase<T, Size>
{
	//put generic vector stuff here
	constexpr SVector() = default;

	template<class Q = T>
	typename std::enable_if_t<(Size == 2), Q> Demote()
	{
		return (*this)[0];
	}

	template<class Q = T>
	typename std::enable_if_t<(Size > 2), SVector<Q, Size - 1>> Demote()
	{
		SVector<Q, Size - 1> ReturnVal;

		for (u32 u = 0; u < Size - 1; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}

	SVector<T, Size + 1> Promote()
	{
		SVector<T, Size + 1> ReturnVal;

		for (u32 u = 0; u < Size; u++)
		{
			ReturnVal[u] = (*this)[u];
		}

		return ReturnVal;
	}

	T& operator[] (const u32 Index)
	{
		T* PointerToMember = reinterpret_cast<T*>(this);
		PointerToMember += Index;
		return *PointerToMember;
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