#pragma once

#include "Policies/Policy.h"

#include "System/SystemContainer.h"

#include "Utilities/RestrictTypes.h"

#include <optional>

class SystemContainer;

class SystemBase
	: public NonCopyable
{
public:
	virtual ~SystemBase() = default;

	virtual void Tick() {}

protected:
	void EnableTick();

	SystemBase() = default;
};

template<typename TPolicySet = Policy::Set<>>
class System
	: public SystemBase
{
public:
	using Super = System<TPolicySet>;
	using Dependencies = typename TPolicySet::template DependenciesRef;

	inline System(Dependencies&& aDependencies);

protected:
	System() = default;
	
	template<
		typename TDependency,
		typename = std::enable_if_t<
			TPolicySet::template HasPolicy_v<TDependency, Policy::EPermission::Write>
			|| TPolicySet::template HasPolicy_v<TDependency, Policy::EPermission::Unrestricted>>>
	constexpr __forceinline TDependency& Get()
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<
		typename TDependency,
		typename = std::enable_if_t<
			TPolicySet::template HasDependency_v<TDependency>>>
		constexpr __forceinline const TDependency& Get() const
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

private:
	Dependencies myDependencies;

	template<typename>
	friend class FunctionalitySystem;
};

template<typename TPolicySet>
inline System<TPolicySet>::System(Dependencies&& aDependencies)
	: myDependencies(std::move(aDependencies))
{}
