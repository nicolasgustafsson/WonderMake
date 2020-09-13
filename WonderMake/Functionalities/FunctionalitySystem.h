#pragma once
#include "Components/ComponentSystem.h"

#include "Functionalities/BaseFunctionality.h"

#include "Object/DependencyDestructor.h"
#include "Object/Object.h"

#include "System/System.h"

#include "Utilities/plf_colony.h"

#include <type_traits>

struct SComponent;

template<typename TFunctionality>
class FunctionalitySystemDelegate;

template<typename TDependency>
using ExtractSystemType =
	// If: Dependency is a functionality
	std::conditional_t<std::is_base_of_v<_BaseFunctionality, TDependency>,
		// Return the FunctionalitySystemDelegate
		FunctionalitySystemDelegate<TDependency>,
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
	FunctionalitySystem(typename CreateSystem<TFunctionality>::Dependencies&& aDependencies);

	TFunctionality& AddFunctionality(Object& aObject, const bool aExplicitlyAdded = true);
	void RemoveFunctionality(TFunctionality& aFunctionality);

	bool IsEmpty() const noexcept;

	virtual void Tick() override;

private:
	plf::colony<TFunctionality>	myFunctionalities;
	DependencyDestructor		myDependencyDestructor;

	template<typename... TDependencies>
	struct DependencyWrapper {};

	template<typename TDependency>
	TDependency& PopulateDependency(Object& aObject) noexcept;
	template<typename... TDependencies>
	typename TFunctionality::Dependencies PopulateDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept;
	typename TFunctionality::Dependencies PopulateDependencies(Object& aObject) noexcept;

	template<typename... TDependencies>
	void RemoveDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept;
	void RemoveDependencies(Object& aObject) noexcept;
};

template<typename TFunctionality>
using FunctionalitySystemDelegateSystem = System < Policy::Set<Policy::Add<FunctionalitySystem<TFunctionality>, Policy::EPermission::Write>>>;

template<typename TFunctionality>
class FunctionalitySystemDelegate final
	: public FunctionalitySystemDelegateSystem<TFunctionality>
{
public:
	FunctionalitySystemDelegate(typename FunctionalitySystemDelegateSystem<TFunctionality>::Dependencies&& aDependencies) noexcept
		: FunctionalitySystemDelegateSystem<TFunctionality>(std::move(aDependencies))
		, myFunctionalityConstructor([this](auto& aObject, const bool aExplicitlyAdded) -> auto&
			{
				return this->Get<FunctionalitySystem<TFunctionality>>().AddFunctionality(aObject, aExplicitlyAdded);
			})
	{}

	TFunctionality& AddFunctionality(Object& aObject, const bool aExplicitlyAdded = true)
	{
		return myFunctionalityConstructor(aObject, aExplicitlyAdded);
	}

private:
	std::function<TFunctionality& (Object&, const bool)> myFunctionalityConstructor;
};

#define REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality) _REGISTER_SYSTEM_IMPL(FunctionalitySystem<aFunctionality>, aFunctionality) _REGISTER_SYSTEM_IMPL(FunctionalitySystemDelegate<aFunctionality>, aFunctionality##_Delegate) 

template<typename TFunctionality>
FunctionalitySystem<TFunctionality>::FunctionalitySystem(typename CreateSystem<TFunctionality>::Dependencies&& aDependencies)
	: CreateSystem<TFunctionality>(std::move(aDependencies))
	, myDependencyDestructor([this](Object& aObject, auto* aFunctionality)
		{
			auto& functionality = *static_cast<TFunctionality*>(static_cast<_BaseFunctionality*>(aFunctionality));

			functionality.Destroy();

			RemoveFunctionality(functionality);

			RemoveDependencies(aObject);
		})
{
	if (typeid(&TFunctionality::Tick) != typeid(&_BaseFunctionality::Tick))
		CreateSystem<TFunctionality>::EnableTick();
}

template<typename TFunctionality>
typename TFunctionality& FunctionalitySystem<TFunctionality>::AddFunctionality(Object& aObject, const bool aExplicitlyAdded)
{
	return aObject.Add<TFunctionality>([this](auto& aObject) -> auto&
	{
		return *myFunctionalities.emplace(aObject, PopulateDependencies(aObject));
	}, myDependencyDestructor, aExplicitlyAdded);
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::RemoveFunctionality(TFunctionality& aFunctionality)
{
	myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(&aFunctionality));
}

template<typename TFunctionality>
bool FunctionalitySystem<TFunctionality>::IsEmpty() const noexcept
{
	return myFunctionalities.empty();
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::Tick()
{
	for (auto&& functionality : myFunctionalities)
		functionality.Tick();
}

template<typename TFunctionality>
template<typename TDependency>
TDependency& FunctionalitySystem<TFunctionality>::PopulateDependency(Object& aObject) noexcept
{
	// TODO(Kevin): This is currently not thread-safe, to make it threadsafe it needs to be run from a job with the same dependencies, but write permission on them.
	if constexpr (std::is_base_of_v<_BaseFunctionality, TDependency>)
	{
		return std::get<FunctionalitySystemDelegate<std::decay_t<TDependency>>&>(this->myDependencies).AddFunctionality(aObject, false);
	}
	else if constexpr (std::is_base_of_v<SComponent, TDependency>)
	{
		return std::get<ComponentSystem<std::decay_t<TDependency>>&>(this->myDependencies).AddComponent(aObject, false);
	}
	else
	{
		return std::get<std::decay_t<TDependency>&>(this->myDependencies);
	}
}

template<typename TFunctionality>
template<typename... TDependencies>
typename TFunctionality::Dependencies FunctionalitySystem<TFunctionality>::PopulateDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept
{
	return std::tie(PopulateDependency<std::decay_t<TDependencies>>(aObject)...);
}

template<typename TFunctionality>
typename TFunctionality::Dependencies FunctionalitySystem<TFunctionality>::PopulateDependencies(Object& aObject) noexcept
{
	return PopulateDependencies(DependencyWrapper<typename TFunctionality::Dependencies>(), aObject);
}

template<typename TFunctionality>
template<typename... TDependencies>
void FunctionalitySystem<TFunctionality>::RemoveDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept
{
	(aObject.Remove<std::decay_t<TDependencies>>(false), ...);
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::RemoveDependencies(Object& aObject) noexcept
{
	RemoveDependencies(DependencyWrapper<typename TFunctionality::Dependencies>(), aObject);
}
