#pragma once
#include <optional>
#include <tuple>
#include <variant>

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

	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(const TCallable& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)aCallable(std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(TCallable& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)aCallable(std::forward<TArgs>(aArgs)...);
	}

	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(const std::optional<TCallable>& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)(*aCallable)(std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(std::optional<TCallable>& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)(*aCallable)(std::forward<TArgs>(aArgs)...);
	}

	template <typename Out>
	static inline void Split(const std::string& aString, char aDelimiter, Out aResult) {
		std::istringstream stringstream(aString);
		std::string item;
		while (std::getline(stringstream, item, aDelimiter)) {
			*aResult++ = item;
		}
	}

	static inline std::vector<std::string> Split(const std::string& aString, char aDelimiter) {
		std::vector<std::string> elems;
		Split(aString, aDelimiter, std::back_inserter(elems));
		return elems;
	}

	namespace detail
	{
		template<typename ...Ts>
		struct MakeVariantImpl
		{

			using Variant = std::variant<Ts...>;

			template<std::size_t Index, typename, typename ...Rest>
			static decltype(auto) make_variant(std::size_t index)
			{
				if (Index == index)
					return Variant{ std::in_place_index_t<Index>{} };

				if constexpr (sizeof...(Rest) != 0)
					return make_variant<Index + 1, Rest...>(index);
				else
					throw std::runtime_error("Invalid variant index");
			}

		};

	} // namespace detail


	template<typename ...Ts>
	decltype(auto) VariantFromIndex(std::size_t aIndex)
	{
		return detail::MakeVariantImpl<Ts...>::template make_variant<0, Ts...>(aIndex);
	}

	template<class... Vs>
	decltype(auto) VariantFromIndex(std::variant<Vs...> const&, std::size_t aIndex)
	{
		return VariantFromIndex<Vs...>(aIndex);
	}
}
