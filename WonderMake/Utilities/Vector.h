#pragma once
#include "Typedefs.h"

template<typename T, u32 Size, typename Enable = void>
struct SVectorBase;

template <typename T, u32 Size>
struct SVectorBase<T, Size, std::enable_if_t<Size == 1>>
{
public:
	T X;
};

template <typename T, u32 Size>
struct SVectorBase<T, Size, std::enable_if_t<Size == 2>>
{
public:
	T X, Y;
};

template <typename T, u32 Size>
struct SVectorBase<T, Size, std::enable_if_t<Size == 3>>
{
public:
	T X, Y, Z;
};

template <typename T, u32 Size>
struct SVectorBase<T, Size, std::enable_if_t<Size == 4>>
{
public:
	T X, Y, Z, W;
};

template <typename T, u32 Size>
struct SVector : SVectorBase<T, Size>
{
	constexpr SVector() = default;

	//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	//template<T... Num, typename Enable = std::enable_if_t<std::integral_constant<u32, std::index_sequence<Num...>::size()>::value == Size>>
	//constexpr SVector(std::index_sequence<Num...> Indices)
	//{
	//	for (auto&& Index : Indices)
	//	{
	//
	//	}
	//}

	T& operator[] (const u32 Index)
	{
		return static_cast<T>(*(this + sizeof(T) * Index));
	}

	//put generic vector stuff here
};

template <typename T>
using SVector2 = SVector<T, 2>;

template <typename T>
using SVector3 = SVector<T, 3>;

template <typename T>
using SVector4 = SVector<T, 4>;


using SVector2f = SVector2<float>;
using SVector3f = SVector3<float>;
using SVector4f = SVector4<float>;