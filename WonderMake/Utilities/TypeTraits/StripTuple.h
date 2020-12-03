#pragma once
#include <tuple>

template<typename>
struct Strip;

template<typename ... T>
struct Dummy
{
	//[Nicos]: its a tuple without the data!
};

template<typename ...T>
struct Strip<std::tuple<T...>>
{
	using type = Dummy<T...>;
};
