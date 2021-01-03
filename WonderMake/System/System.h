#pragma once

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"

#include <optional>

class SystemBase
	: public NonCopyable
{
public:
	virtual ~SystemBase() = default;

protected:
	constexpr SystemBase() noexcept = default;
};

template<typename TPolicySet = Policy::Set<>>
class System
	: public SystemBase
{
public:
	using Super = System<TPolicySet>;
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
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires TPolicySet::template HasDependency_v<TDependency>
	constexpr __forceinline [[nodiscard]] const TDependency& Get() const
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

private:
	static thread_local std::optional<Dependencies> myInjectedDependencies;

	Dependencies myDependencies;

	template<typename>
	friend class FunctionalitySystem;
};

template<typename TPolicySet>
thread_local std::optional<typename System<TPolicySet>::Dependencies> System<TPolicySet>::myInjectedDependencies;
