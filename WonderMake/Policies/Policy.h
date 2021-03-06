#pragma once

#include <tuple>
#include <vector>
#include <type_traits>

#include "Policies/SystemId.h"

struct Policy final
{
	enum class EPermission
	{
		Write,
		Read
	};

	SystemId		myDependencyId;
	EPermission		myPermission = EPermission::Read;

	template<typename TDependency>
	[[nodiscard]] inline static Policy Create(const EPermission aPermission) noexcept
	{
		return { SystemId::Create<TDependency>(), aPermission };
	}

	[[nodiscard]] inline bool Conflicts(const Policy& aOther) const noexcept
	{
		return myDependencyId == aOther.myDependencyId
			&& (myPermission == EPermission::Write
				|| aOther.myPermission == EPermission::Write);
	}

	template<typename TDependency, Policy::EPermission TPermission>
	struct Add
	{
		using Dependency = typename std::decay_t<TDependency>;
		static constexpr auto Permission = TPermission;
	};

	template<typename... TPolicies>
	struct Set
	{
	private:
		template<typename TPolicy>
		using ExtractDependency = typename TPolicy::Dependency;

	public:
		using Dependencies = std::tuple<ExtractDependency<TPolicies>...>;
		using DependenciesRef = std::tuple<ExtractDependency<TPolicies>&...>;

		template<template<typename> typename TExpectedType>
		using ExtractDependencies = std::tuple<TExpectedType<ExtractDependency<TPolicies>>...>;

		template<template<typename...> typename TPolicyContainer>
		using ExtractPolicies = TPolicyContainer<TPolicies...>;

		[[nodiscard]] inline static std::vector<Policy> GetPolicies()
		{
			std::vector<Policy> policies;

			(policies.emplace_back(Policy::Create<TPolicies::Dependency>(TPolicies::Permission)), ...);

			return policies;
		}

		template<typename TDependency>
		[[nodiscard]] inline static constexpr bool HasDependency() noexcept
		{
			return (PolicyHasDependency<TPolicies, TDependency>() || ...);
		}
		template<typename TDependency, EPermission TPermission>
		[[nodiscard]] inline static constexpr bool HasPolicy() noexcept
		{
			return ((PolicyHasDependency<TPolicies, TDependency>() && PolicyHasPermission<TPolicies, TPermission>()) || ...);
		}

		template<typename TDependency>
		static constexpr bool HasDependency_v = HasDependency<TDependency>();
		template<typename TDependency, EPermission TPermission>
		static constexpr bool HasPolicy_v = HasPolicy<TDependency, TPermission>();
	};

private:
	template<typename... TPolicySets>
	struct ConcatPolicySets
	{
		auto operator()()
		{
			return std::tuple_cat(typename TPolicySets::template ExtractPolicies<std::tuple>()...);
		}
	};
	template<typename TPolicyTuple>
	struct ConcatPolicies {};
	template<typename... TPolicies>
	struct ConcatPolicies<std::tuple<TPolicies...>>
	{
		using Result = Policy::Set<TPolicies...>;
	};

public:
	template<typename... TPolicySets>
	using Concat = typename ConcatPolicies<std::invoke_result_t<ConcatPolicySets<TPolicySets...>>>::Result;

private:
	template<typename TPolicy, typename TDependency>
	[[nodiscard]] inline static constexpr bool PolicyHasDependency() noexcept
	{
		return std::is_same_v<TPolicy::Dependency, std::decay_t<TDependency>>;
	}
	template<typename TPolicy, EPermission TPermission>
	[[nodiscard]] inline static constexpr bool PolicyHasPermission() noexcept
	{
		return TPermission == TPolicy::Permission;
	}

};

template<typename TDependency, Policy::EPermission TPermission>
using PAdd = Policy::Add<TDependency, TPermission>;

static constexpr auto PWrite		= Policy::EPermission::Write;
static constexpr auto PRead			= Policy::EPermission::Read;
