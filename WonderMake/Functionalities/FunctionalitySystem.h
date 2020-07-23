#pragma once
#include "Functionalities/BaseFunctionality.h"

#include "System/System.h"

#include "Utilities/plf_colony.h"

#include <type_traits>

class Object;
struct SComponent;

template<typename TFunctionality>
class FunctionalitySystem;

template<typename TDependency>
using ExtractSystemType =
	// If: Dependency is a functionality
	std::conditional_t<std::is_base_of_v<_BaseFunctionality, TDependency>,
		// Return the FunctionalitySystem
		FunctionalitySystem<TDependency>,
	// ElseIf: Dependency is a component
	std::conditional_t<std::is_base_of_v<SComponent, TDependency>,
		// Return the ComponentSystem
		ComponentSystem<TDependency>,
	// Else:
		// Return the Dependency as is
		TDependency>>;

template<typename TPolicy>
using ConvertPolicy =
	Policy::Add<
		ExtractSystemType<typename TPolicy::Dependency>,
		TPolicy::Permission>;

template<typename... TFunctionalityPolicies>
using CreatePolicySet =
	Policy::Set<
		ConvertPolicy<TFunctionalityPolicies>...>;

template<typename TPolicySet>
using ConvertPolicySet =
	typename TPolicySet::template ExtractPolicies<CreatePolicySet>;

template<typename TFunctionality>
using CreateSystem = System<ConvertPolicySet<typename TFunctionality::PolicySet>>;

template<typename TFunctionality>
class FunctionalitySystem final
	: public CreateSystem<TFunctionality>
{
public:
	FunctionalitySystem(typename CreateSystem<TFunctionality>::Dependencies&& aDependencies) noexcept;

	TFunctionality& AddFunctionality(Object& aObject);
	void RemoveFunctionality(TFunctionality& aFunctionality);

	bool IsEmpty() const;

	virtual void Tick() noexcept override;

private:
	plf::colony<TFunctionality> myFunctionalities;
};

#define REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality) _REGISTER_SYSTEM_IMPL(FunctionalitySystem<aFunctionality>, aFunctionality)

template<typename TFunctionality>
FunctionalitySystem<TFunctionality>::FunctionalitySystem(typename CreateSystem<TFunctionality>::Dependencies&& aDependencies) noexcept
	: CreateSystem<TFunctionality>(std::move(aDependencies))
{
	if (typeid(&TFunctionality::Tick) != typeid(&_BaseFunctionality::Tick))
		CreateSystem<TFunctionality>::EnableTick();
}

template<typename TFunctionality>
typename TFunctionality& FunctionalitySystem<TFunctionality>::AddFunctionality(Object& aObject)
{
	return (*myFunctionalities.emplace(aObject));
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::RemoveFunctionality(TFunctionality& aFunctionality)
{
	myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(&aFunctionality));
}

template<typename TFunctionality>
bool FunctionalitySystem<TFunctionality>::IsEmpty() const
{
	return myFunctionalities.empty();
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::Tick() noexcept
{
	for (auto&& functionality : myFunctionalities)
		functionality.Tick();
}
