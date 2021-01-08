#pragma once

#include <tuple>
#include <type_traits>

template<typename... TContent>
struct Cargo
{
	template<typename TDependency>
	TDependency& Get() const
	{
		return std::get<TDependency&>(Dependencies);
	}

	std::tuple<TContent&...> Dependencies;
};

template<typename... TContent>
Cargo<TContent...> WrapCargo(TContent&... aArgs)
{
	return Cargo<TContent...>{ std::tie(aArgs...) };
}
