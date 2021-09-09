#pragma once
#include "Components/ComponentSystem.h"

#include "Functionalities/BaseFunctionality.h"

#include "Object/DependencyDestructor.h"
#include "Object/Object.h"

#include "Scheduling/ScheduleSystem.h"

#include "System/System.h"

#include "Utilities/plf_colony.h"

#include <type_traits>

struct SComponent;

template<typename TFunctionality>
class FunctionalitySystemDelegate;

namespace _Impl
{
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
	using ConvertPolicy = PAdd<ExtractSystemType<typename TPolicy::Dependency>, TPolicy::Permission>;

	template<typename... TFunctionalityPolicies>
	using CreatePolicySet = Policy::Set<PAdd<ScheduleSystem, PWrite>, ConvertPolicy<TFunctionalityPolicies>...>;

	template<typename TPolicySet>
	using ConvertPolicySet = typename TPolicySet::template ExtractPolicies<CreatePolicySet>;

	template<typename TFunctionality>
	using GetFunctionalitySystem = System<ConvertPolicySet<typename TFunctionality::PolicySet>>;
}

template<typename TFunctionality>
class FunctionalitySystem final
	: public _Impl::GetFunctionalitySystem<TFunctionality>
{
public:
	using Super = _Impl::GetFunctionalitySystem<TFunctionality>;

	inline FunctionalitySystem()
		: myDependencyDestructor([this](Object& aObject, auto* aFunctionality)
			{
				auto& functionality = *static_cast<TFunctionality*>(static_cast<_BaseFunctionality*>(aFunctionality));

				RemoveFunctionality(functionality);

				RemoveDependencies(aObject);
			})
	{
		if (typeid(&TFunctionality::Tick) != typeid(&_BaseFunctionality::Tick))
			Super::template Get<ScheduleSystem>().ScheduleRepeating<_Impl::ConvertPolicySet<typename Super::PolicySet>>([this]() { Tick(); });
	}

	inline [[nodiscard]] TFunctionality& AddFunctionality(Object& aObject, const bool aExplicitlyAdded = true)
	{
		TFunctionality& added = aObject.Add<TFunctionality>([this](auto& aObject) -> auto&
		{
			TFunctionality::InjectDependencies(PopulateDependencies(aObject));

			return *myFunctionalities.emplace();
		}, myDependencyDestructor, aExplicitlyAdded);

		added.OnNewObject(aObject);
		
		return added;
	}

	inline void RemoveFunctionality(TFunctionality& aFunctionality)
	{
		if (myInTick)
		{
			myFunctionalitiesToErase.insert(&aFunctionality);
			return;
		}

		myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(&aFunctionality));
	}

	inline bool IsEmpty() const noexcept
	{
		return myFunctionalities.empty();
	}

	inline [[nodiscard]] void Tick()
	{
		myInTick = true;
		plf::colony<TFunctionality*> functionalitiesReferenced;

		for (auto&& functionality : myFunctionalities)
			functionalitiesReferenced.insert(&functionality);

		for (auto&& functionality : functionalitiesReferenced)
			functionality->Tick();

		myInTick = false;

		for (auto&& functionality : myFunctionalitiesToErase)
			myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(functionality));

		myFunctionalitiesToErase.clear();
	}

private:

	bool myInTick = false;
	plf::colony<TFunctionality>	myFunctionalities;
	plf::colony<TFunctionality*> myFunctionalitiesToErase;
	DependencyDestructor		myDependencyDestructor;

	template<typename... TDependencies>
	struct DependencyWrapper {};

	template<typename TDependency>
	inline [[nodiscard]] TDependency& PopulateDependency(Object& aObject) noexcept
	{
		// TODO(Kevin): This is currently not thread-safe, to make it threadsafe it needs to be run from a job with the same dependencies, but write permission on them.
		if constexpr (std::is_base_of_v<_BaseFunctionality, TDependency>)
			return std::get<FunctionalitySystemDelegate<std::decay_t<TDependency>>&>(this->myDependencies).AddFunctionality(aObject, false);
		else if constexpr (std::is_base_of_v<SComponent, TDependency>)
			return std::get<ComponentSystem<std::decay_t<TDependency>>&>(this->myDependencies).AddComponent(aObject, false);
		else
			return std::get<std::decay_t<TDependency>&>(this->myDependencies);
	}
	template<typename... TDependencies>
	inline [[nodiscard]] typename TFunctionality::Dependencies PopulateDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept
	{
		return std::tie(PopulateDependency<std::decay_t<TDependencies>>(aObject)...);
	}
	inline [[nodiscard]] typename TFunctionality::Dependencies PopulateDependencies(Object& aObject) noexcept
	{
		return PopulateDependencies(DependencyWrapper<typename TFunctionality::Dependencies>(), aObject);
	}

	template<typename... TDependencies>
	inline void RemoveDependencies(DependencyWrapper<std::tuple<TDependencies...>>, Object& aObject) noexcept
	{
		(aObject.Remove<std::decay_t<TDependencies>>(false), ...);
	}
	inline void RemoveDependencies(Object& aObject) noexcept
	{
		RemoveDependencies(DependencyWrapper<typename TFunctionality::Dependencies>(), aObject);
	}
};

template<typename TFunctionality>
using FunctionalitySystemDelegateSystem = System<Policy::Set<PAdd<FunctionalitySystem<TFunctionality>, PWrite>>>;

template<typename TFunctionality>
class FunctionalitySystemDelegate final
	: public FunctionalitySystemDelegateSystem<TFunctionality>
{
public:
	inline FunctionalitySystemDelegate() noexcept
		: myFunctionalityConstructor([this](auto& aObject, const bool aExplicitlyAdded) -> auto&
			{
				return this->Get<FunctionalitySystem<TFunctionality>>().AddFunctionality(aObject, aExplicitlyAdded);
			})
	{}

	inline [[nodiscard]] TFunctionality& AddFunctionality(Object& aObject, const bool aExplicitlyAdded = true)
	{
		return myFunctionalityConstructor(aObject, aExplicitlyAdded);
	}

private:
	UniqueFunction<TFunctionality& (Object&, const bool)> myFunctionalityConstructor;
};

#define REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality) _REGISTER_SYSTEM_IMPL(FunctionalitySystem<aFunctionality>, aFunctionality) _REGISTER_SYSTEM_IMPL(FunctionalitySystemDelegate<aFunctionality>, aFunctionality##_Delegate) 
