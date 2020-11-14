#pragma once

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"

class SystemBase
	: public NonCopyable
{
public:
	virtual ~SystemBase() = default;

	virtual void Tick() {}

protected:
	void EnableTick();

	constexpr SystemBase() noexcept = default;
};

template<typename TPolicySet = Policy::Set<>>
class System
	: public SystemBase
{
public:
	using Super = System<TPolicySet>;
	using Dependencies = typename TPolicySet::template DependenciesRef;

	inline constexpr System(Dependencies&& aDependencies) noexcept
		: myDependencies(std::move(aDependencies))
	{}

protected:
	constexpr System() noexcept = default;
	
	template<typename TDependency> requires
		TPolicySet::template HasPolicy_v<TDependency, PWrite>
	constexpr __forceinline TDependency& Get()
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires
		TPolicySet::template HasDependency_v<TDependency>
	constexpr __forceinline const TDependency& Get() const
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

private:
	Dependencies myDependencies;

	template<typename>
	friend class FunctionalitySystem;
};
