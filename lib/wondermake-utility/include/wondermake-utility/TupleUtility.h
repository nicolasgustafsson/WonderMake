#pragma once

#include <tuple>
#include <type_traits>

namespace Utility
{

	template <typename T,
		typename TIter = decltype(std::begin(std::declval<T>())),
		typename = decltype(std::end(std::declval<T>()))>
		constexpr auto Enumerate(T&& iterable)
	{
		struct iterator
		{
			size_t i;
			TIter iter;
			bool operator != (const iterator& other) const { return iter != other.iter; }
			void operator ++ () { ++i; ++iter; }
			auto operator * () const { return std::tie(i, *iter); }
		};
		struct iterable_wrapper
		{
			T iterable;
			auto begin() { return iterator{ 0, std::begin(iterable) }; }
			auto end() { return iterator{ 0, std::end(iterable) }; }
		};
		return iterable_wrapper{ std::forward<T>(iterable) };
	}

}

template <class T, class Tuple>
struct TupleIndex;

template <class T, class... Types>
struct TupleIndex<T, std::tuple<T, Types...>>
{
	static const std::size_t Index = 0;
};

template <class T, class U, class... Types>
struct TupleIndex<T, std::tuple<U, Types...>>
{
	static const std::size_t Index = 1 + TupleIndex<T, std::tuple<Types...>>::Index;
};

template <typename TType, typename TTuple>
struct TupleContainsType
	: std::false_type
{};

template<typename TType, typename TRequiredFront, typename... TRequiredOthers>
struct TupleContainsType<TType, std::tuple<TRequiredFront, TRequiredOthers...>>
	: std::bool_constant<std::is_same_v<TType, TRequiredFront> || TupleContainsType<TType, std::tuple<TRequiredOthers...>>::value>
{};

template<typename TType, typename TTuple>
inline constexpr bool TupleContainsType_v = TupleContainsType<TType, TTuple>::value;

template <typename TTuple, typename TRequiredTypesTuple>
struct TupleContainsAllTypesInTuple
	: std::false_type
{};

template<typename TRequiredTypesTuple, typename... TTypes>
struct TupleContainsAllTypesInTuple<std::tuple<TTypes...>, TRequiredTypesTuple>
	: std::bool_constant<std::conjunction_v<TupleContainsType<TTypes, TRequiredTypesTuple>...>>
{};

template<typename TRequiredTypesTuple, typename TTuple>
inline constexpr bool TupleContainsAllTypesInTuple_v = TupleContainsAllTypesInTuple<TTuple, TRequiredTypesTuple>::value;
