#pragma once

#include "SystemTypeId.h"

#include <tuple>
#include <vector>
#include <type_traits>

struct Policy final
{
	enum class EPermission
	{
		Write,
		Read
	};

	SystemTypeId	myDependencyId;
	EPermission		myPermission = EPermission::Read;

	inline [[nodiscard]] bool operator==(const Policy&) const noexcept = default;

	template<typename TDependency>
	[[nodiscard]] inline static Policy Create(const EPermission aPermission) noexcept
	{
		return { SystemTypeId::Create<TDependency>(), aPermission };
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
	public:
		template<template<typename...> typename TPolicyContainer>
		using ExtractPolicies = TPolicyContainer<TPolicies...>;

		[[nodiscard]] inline static std::vector<Policy> GetPolicies()
		{
			std::vector<Policy> policies;

			(policies.emplace_back(Policy::Create<typename TPolicies::Dependency>(TPolicies::Permission)), ...);

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
		return std::is_same_v<typename TPolicy::Dependency, std::decay_t<TDependency>>;
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
