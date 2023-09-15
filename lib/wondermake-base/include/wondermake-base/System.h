#pragma once

#include "SystemPolicy.h"
#include "SystemTraits.h"

#include "wondermake-utility/Executor.h"
#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/SharedReference.h"
#include "wondermake-utility/Utility.h"

#include <optional>
#include <type_traits>

class SystemBase
	: public NonCopyable
{
public:
	virtual ~SystemBase() = default;

protected:
	constexpr SystemBase() noexcept = default;
};

template<typename TPolicySet = Policy::Set<>, typename TTraitSet = SystemTraits::Set<>>
class SystemSub
	: public SystemBase
{
private:
	template<typename TPolicySet>
	struct SDependencyTuple
	{
		using type = std::tuple<>;
	};

	template<typename... TPolicies>
	struct SDependencyTuple<Policy::Set<TPolicies...>>
	{
		using type = std::tuple<SharedReference<typename TPolicies::Dependency>...>;
	};

public:
	using PolicySet = TPolicySet;
	using TraitSet = TTraitSet;
	using Dependencies = SDependencyTuple<TPolicySet>::type;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

	virtual void Initialize() {};

	inline InlineExecutor GetExecutor()
	{
		return InlineExecutor();
	}

protected:
	inline SystemSub() noexcept
		: myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedDependencies.reset();
	}

	template<typename TDependency> requires TPolicySet::template HasPolicy_v<TDependency, PWrite>
	constexpr __forceinline [[nodiscard]] TDependency& Get()
	{
		return *std::get<SharedReference<std::decay_t<TDependency>>>(myDependencies);
	}

	template<typename TDependency> requires TPolicySet::template HasDependency_v<TDependency>
	constexpr __forceinline [[nodiscard]] const TDependency& Get() const
	{
		return *std::get<SharedReference<std::decay_t<TDependency>>>(myDependencies);
	}

private:
	static thread_local std::optional<Dependencies> myInjectedDependencies;

	Dependencies myDependencies;

	template<typename>
	friend class FunctionalitySystem;
};

template<typename TPolicySet, typename TTraitSet>
thread_local std::optional<typename SystemSub<TPolicySet, TTraitSet>::Dependencies> SystemSub<TPolicySet, TTraitSet>::myInjectedDependencies;

class SystemAbstracted
	: public NonCopyable
{
public:
	virtual ~SystemAbstracted() = default;

protected:
	constexpr SystemAbstracted() noexcept = default;
};

template<typename TPolicySet = Policy::Set<>, typename TTraitSet = SystemTraits::Set<>>
class System
	: public SystemAbstracted
	, public SystemSub<TPolicySet, TTraitSet>
{
private:
	using InheritedSystem = SystemSub<TPolicySet, TTraitSet>;

public:
	using Super = System<TPolicySet, TTraitSet>;
	using Dependencies = InheritedSystem::Dependencies;

	using InheritedSystem::InheritedSystem;
};

template<typename TSystem>
[[nodiscard]] inline SharedReference<TSystem> MakeSystem(typename TSystem::Dependencies aDependencies)
{
	TSystem::InjectDependencies(std::move(aDependencies));

	return MakeSharedReference<TSystem>();
}

template<typename TSystem>
concept CNamedSystem = requires()
{
	{ TSystem::TypeName() } -> std::convertible_to<std::string_view>;
};

template<typename TSystem>
[[nodiscard]] inline std::string_view GetSystemTypeName() noexcept
{
	if constexpr (CNamedSystem<TSystem>)
		return TSystem::TypeName();
	else
		return Utility::GetTypeName<TSystem>();
}
