#pragma once
#include <type_traits>
#include <tuple>

template<typename>
struct ParameterPackFromTuple;

template<typename ... T>
struct ParameterPack
{
	//[Nicos]: its a tuple without the data and the slow to compile functions!
};

template<typename ...T>
struct ParameterPackFromTuple<std::tuple<T...>>
{
	using type = ParameterPack<T...>;
};

template<class T = ParameterPack<>, class...>
struct Concatenate { using type = T; };

template<class... T1, class... T2, class... Ts>
struct Concatenate<ParameterPack<T1...>, ParameterPack<T2...>, Ts...>
	: Concatenate<ParameterPack<T1..., T2...>, Ts...> {};

template<class... Ts>
using Concatenate_t = typename Concatenate<Ts...>::type;

template <typename T, typename... Ts>
struct Unique : std::type_identity<T> {};

template <typename... Ts, typename U, typename... Us>
struct Unique<ParameterPack<Ts...>, U, Us...>
	: std::conditional_t<(std::is_same_v<U, Ts> || ...)
	, Unique<ParameterPack<Ts...>, Us...>
	, Unique<ParameterPack<Ts..., U>, Us...>> {};

template <typename... Ts>
using UniqueParameterPack = typename Unique<ParameterPack<>, Ts...>::type;

template <typename ... TPack>
auto UniqueParameterPackFun(ParameterPack<TPack...>)
{
	return UniqueParameterPack<TPack...>();
}

template <template <typename> class Pred, typename TUPLE, typename Res = ParameterPack<>>
struct TypeFilter;

template <template <typename> class Pred, typename Res>
struct TypeFilter<Pred, ParameterPack<>, Res>
{
	using type = Res;
};

template <template <typename> class Pred, typename T, typename ... Ts, typename ... TRes>
struct TypeFilter<Pred, ParameterPack<T, Ts...>, ParameterPack<TRes...>> :
	TypeFilter<Pred,
	ParameterPack<Ts...>,
	std::conditional_t<Pred<T>::value,
	ParameterPack<TRes..., T>,
	ParameterPack<TRes...>>>
{
};

template <typename ... TPack>
constexpr size_t ParameterPackSize(ParameterPack<TPack...>)
{
	return sizeof...(TPack);
}
