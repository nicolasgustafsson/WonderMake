#pragma once

#include <vector>

#include "Policies/SystemId.h"

struct Policy final
{
	enum class EPermission
	{
		Write,
		Read,
		Unrestricted
	};

	SystemId		myDependencyId;
	EPermission		myPermission = EPermission::Read;

	template<typename TDependency>
	[[nodiscard]] inline static Policy Create(const EPermission aPermission) noexcept;

	[[nodiscard]] inline bool Conflicts(const Policy& aOther) const noexcept;

	template<typename TDependency, Policy::EPermission TPermission>
	struct Add
	{
		using Dependency = TDependency;
		static constexpr auto Permission = TPermission;
	};

	template<typename... TPolicies>
	struct Set
	{
		[[nodiscard]] inline static std::vector<Policy> GetPolicies();

		template<typename TDependency>
		[[nodiscard]] inline static constexpr bool HasDependency() noexcept;
		template<typename TDependency, EPermission TPermission>
		[[nodiscard]] inline static constexpr bool HasPolicy() noexcept;

		template<typename TDependency>
		static constexpr bool HasDependency_v = HasDependency<TDependency>();
		template<typename TDependency, EPermission TPermission>
		static constexpr bool HasPolicy_v = HasPolicy<TDependency, TPermission>();
	};

private:
	template<typename TPolicy, typename TDependency>
	[[nodiscard]] inline static constexpr bool PolicyHasDependency() noexcept;
	template<typename TPolicy, EPermission TPermission>
	[[nodiscard]] inline static constexpr bool PolicyHasPermission() noexcept;
};

template<typename TDependency>
[[nodiscard]] inline static Policy Policy::Create(const EPermission aPermission) noexcept
{
	return { SystemId::Create<TDependency>(), aPermission };
}

[[nodiscard]] inline bool Policy::Conflicts(const Policy& aOther) const noexcept
{
	return myPermission != EPermission::Unrestricted
		&& aOther.myPermission != EPermission::Unrestricted
		&& myDependencyId == aOther.myDependencyId
		&& (myPermission == EPermission::Write
			|| aOther.myPermission == EPermission::Write);
}

template<typename... TPolicies>
[[nodiscard]] inline std::vector<Policy> Policy::Set<TPolicies...>::GetPolicies()
{
	std::vector<Policy> policies;

	(policies.emplace_back(Policy::Create<TPolicies::Dependency>(TPolicies::Permission)), ...);

	return policies;
}

template<typename... TPolicies>
template<typename TDependency>
[[nodiscard]] inline static constexpr bool Policy::Set<TPolicies...>::HasDependency() noexcept
{
	return (PolicyHasDependency<TPolicies, TDependency>() || ...);
}

template<typename... TPolicies>
template<typename TDependency, Policy::EPermission TPermission>
[[nodiscard]] inline static constexpr bool Policy::Set<TPolicies...>::HasPolicy() noexcept
{
	return ((PolicyHasDependency<TPolicies, TDependency>() && PolicyHasPermission<TPolicies, TPermission>()) || ...);
}

template<typename TPolicy, typename TDependency>
[[nodiscard]] inline static constexpr bool Policy::PolicyHasDependency() noexcept
{
	return std::is_same_v<TPolicy::Dependency, TDependency>;
}

template<typename TPolicy, Policy::EPermission TPermission>
[[nodiscard]] inline static constexpr bool Policy::PolicyHasPermission() noexcept
{
	return TPermission == TPolicy::Permission;
}