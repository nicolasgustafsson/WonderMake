#pragma once

#include "wondermake-base/BaseFunctionality.h"
#include "wondermake-base/ComponentSystem.h"
#include "wondermake-base/Object.h"
#include "wondermake-base/ObjectDependencyDestructor.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/System.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/plf_colony.h"
#include "wondermake-utility/SharedReference.h"

#include <functional>
#include <memory>
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
	using GetFunctionalitySystem = System<ConvertPolicySet<typename TFunctionality::PolicySet>, typename TFunctionality::SystemTraits>;
}

template<typename TFunctionality>
class FunctionalitySystem final
	: public _Impl::GetFunctionalitySystem<TFunctionality>
	, public std::enable_shared_from_this<FunctionalitySystem<TFunctionality>>
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
			Super::template Get<ScheduleSystem>()
				.ScheduleRepeating(this->GetExecutor(), [this]() { Tick(); })
				.Detach();
	}

	inline [[nodiscard]] TFunctionality& AddFunctionality(Object& aObject, const bool aExplicitlyAdded = true)
	{
		return aObject.Add<TFunctionality>([this](auto& aObject) -> auto&
		{
			TFunctionality::InjectDependencies(PopulateDependencies(aObject));

			return *myFunctionalities.emplace();
		}, std::shared_ptr<ObjectDependencyDestructor>(this->shared_from_this(), &myDependencyDestructor), aExplicitlyAdded);
	}
	inline void RemoveFunctionality(TFunctionality& aFunctionality)
	{
		myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(&aFunctionality));
	}

	inline bool IsEmpty() const noexcept
	{
		return myFunctionalities.empty();
	}

	inline [[nodiscard]] void Tick()
	{
		for (auto&& functionality : myFunctionalities)
			functionality.TFunctionality::Tick();
	}

private:
	plf::colony<TFunctionality>	myFunctionalities;
	ObjectDependencyDestructor	myDependencyDestructor;

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
using FunctionalitySystemDelegateSystem = System<Policy::Set<PAdd<FunctionalitySystem<TFunctionality>, PWrite>>, typename TFunctionality::SystemTraits>;

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
	std::function<TFunctionality& (Object&, const bool)> myFunctionalityConstructor;
};

#define WM_REGISTER_FUNCTIONALITY_SYSTEM(aFunctionality) _WM_REGISTER_SYSTEM_IMPL(FunctionalitySystem<aFunctionality>, aFunctionality) _WM_REGISTER_SYSTEM_IMPL(FunctionalitySystemDelegate<aFunctionality>, aFunctionality##_Delegate) 
