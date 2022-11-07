#pragma once

#include "Functionalities/BaseFunctionality.h"
#include "Functionalities/FunctionalitySystem.h"

#include <optional>

#define REGISTER_FUNCTIONALITY(aFunctionality) REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality)

class Object;

template<
	typename TPolicySet = Policy::Set<>,
	typename TSystemTraits = SystemTraits::Set<>>
class Functionality
	: public _BaseFunctionality
{
public:
	using Super = Functionality<TPolicySet, TSystemTraits>;
	using Dependencies = typename TPolicySet::DependenciesRef;
	using PolicySet = TPolicySet;
	using SystemTraits = TSystemTraits;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

	inline InlineExecutor GetExecutor()
	{
		return InlineExecutor();
	}

	template<typename TDependency> requires
		TPolicySet::template HasPolicy_v<TDependency, PWrite>
		constexpr __forceinline TDependency& Get() noexcept
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires
		TPolicySet::template HasDependency_v<TDependency>
		constexpr __forceinline const TDependency& Get() const noexcept
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

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

template<typename TPolicySet, typename TSystemTraits>
thread_local std::optional<typename Functionality<TPolicySet, TSystemTraits>::Dependencies> Functionality<TPolicySet, TSystemTraits>::myInjectedDependencies;
