#pragma once

#include "Functionalities/BaseFunctionality.h"
#include "Functionalities/FunctionalitySystem.h"

#include <optional>

#define REGISTER_FUNCTIONALITY(aFunctionality) REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality)

class Object;


template<typename ... TPolicies>
class Functionality
	: public _BaseFunctionality
{
public:
	using TPolicySet = decltype(ConvertToPolicySet<TPolicies...>());
	using Super = Functionality<TPolicySet>;
	using Dependencies = typename TPolicySet::DependenciesRef;
	using PolicySet = TPolicySet;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

#pragma warning( push )
#pragma warning( disable : 4702 )

	template<typename TDependency> requires
		TPolicySet::template HasPolicy_v<TDependency, PWrite>
		constexpr __forceinline TDependency& Get() noexcept
	{
		if constexpr (std::is_base_of_v<SystemBase, TDependency>)
			return std::get<std::decay_t<TDependency>&>(myDependencies).Resolve<TDependency>();

		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires
		TPolicySet::template HasDependency_v<TDependency>
		constexpr __forceinline const TDependency& Get() const noexcept
	{
		if constexpr (std::is_base_of_v<SystemBase, TDependency>)
			return std::get<std::decay_t<TDependency>&>(myDependencies).Resolve<TDependency>();

		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}
#pragma warning( pop )


protected:
	Functionality()
		: myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedDependencies.reset();
	}

private:
	static thread_local std::optional<Dependencies> myInjectedDependencies;

	Dependencies myDependencies;
};

template<typename  ... TPolicies>
thread_local std::optional<typename Functionality<TPolicies...>::Dependencies> Functionality<TPolicies...>::myInjectedDependencies;
