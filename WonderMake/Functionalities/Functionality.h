#pragma once

#include "Functionalities/BaseFunctionality.h"
#include "Functionalities/FunctionalitySystem.h"

#define REGISTER_FUNCTIONALITY(aFunctionality) REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality)

class Object;

template<
	typename TSelfType,
	typename TPolicySet = Policy::Set<>>
class Functionality
	: public _BaseFunctionality
{
public:
	using Super = Functionality<TSelfType, TPolicySet>;
	using Dependencies = typename TPolicySet::DependenciesRef;
	using PolicySet = TPolicySet;

	Functionality(Object& aObject, Dependencies&& aDependencies);
	inline virtual void Destroy() override final;

	template<typename TDependency> requires
		TPolicySet::template HasPolicy_v<TDependency, Policy::EPermission::Write>
		|| TPolicySet::template HasPolicy_v<TDependency, Policy::EPermission::Unrestricted>
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

private:
	friend TSelfType;

	Dependencies myDependencies;
};

template<typename TSelfType, typename TPolicySet>
void Functionality<TSelfType, TPolicySet>::Destroy()
{
	class ImpulseFunctionality;

	if constexpr (TPolicySet::template HasDependency_v<ImpulseFunctionality>)
		Get<ImpulseFunctionality>().UnsubscribeAll(*this);
}

template<typename TSelfType, typename TPolicySet>
Functionality<TSelfType, TPolicySet>::Functionality(Object& /*aObject*/, Dependencies&& aDependencies)
	: myDependencies(std::move(aDependencies))
{}
