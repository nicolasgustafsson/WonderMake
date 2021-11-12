#pragma once

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"

#include <optional>

class SystemBase
	: public NonCopyable
{
public:
	virtual ~SystemBase() = default;

	template<typename T>
	T& Resolve()
	{
		return *(static_cast<T*>(this));
	}

	template<typename T>
	T& Resolve() const
	{
		return *(static_cast<const T*>(this));
	}

protected:
	constexpr SystemBase() noexcept = default;
};

template<typename ... TPolicies>
class System
	: public SystemBase
{
public:
	using TPolicySet = decltype(ConvertToPolicySet<TPolicies...>());
	using Super = System<TPolicySet>;
	using PolicySet = TPolicySet;
	using Dependencies = typename TPolicySet::template DependenciesRef;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

protected:

	inline System() noexcept
		: myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedDependencies.reset();
	}

	template<typename TDependency> requires TPolicySet::template HasPolicy_v<TDependency, PWrite>
	constexpr __forceinline [[nodiscard]] TDependency& Get()
	{
		TDependency& ref = std::get<std::decay_t<TDependency>&>(myDependencies);

		return ref.Resolve<TDependency>();
	}

	template<typename TDependency> requires TPolicySet::template HasDependency_v<TDependency>
	constexpr __forceinline [[nodiscard]] const TDependency& Get() const
	{
		TDependency& ref = std::get<std::decay_t<TDependency>&>(myDependencies);

		return ref.Resolve<TDependency>();
	}

private:
	static thread_local std::optional<Dependencies> myInjectedDependencies;

	Dependencies myDependencies;

	template<typename>
	friend class FunctionalitySystem;
};

template<typename ... TPolicies>
thread_local std::optional<typename System<TPolicies...>::Dependencies> System<TPolicies...>::myInjectedDependencies;
