#pragma once
#include <tuple>

namespace Utility
{
	template <typename T,
		typename TIter = decltype(std::begin(std::declval<T>())),
		typename = decltype(std::end(std::declval<T>()))>
		constexpr auto Enumerate(T && iterable)
	{
		struct iterator
		{
			size_t i;
			TIter iter;
			bool operator != (const iterator & other) const { return iter != other.iter; }
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

	template <class T, class Tuple>
	struct TupleIndex;

	template <class T, class... Types>
	struct TupleIndex<T, std::tuple<T, Types...>> {
		static const std::size_t Index = 0;
	};

	template <class T, class U, class... Types>
	struct TupleIndex<T, std::tuple<U, Types...>> {
		static const std::size_t Index = 1 + TupleIndex<T, std::tuple<Types...>>::Index;
	};
}