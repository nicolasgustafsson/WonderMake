#pragma once

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
	[[nodiscard]] inline static Policy Create(const EPermission aPermission) noexcept;

	[[nodiscard]] inline bool Conflicts(const Policy& aOther) const noexcept;
};

template<typename TDependency>
[[nodiscard]] inline static Policy Policy::Create(const EPermission aPermission) noexcept
{
	return { SystemId::Create<TDependency>(), aPermission };
}

[[nodiscard]] inline bool Policy::Conflicts(const Policy& aOther) const noexcept
{
	return myDependencyId == aOther.myDependencyId
		&& (myPermission == EPermission::Write
			|| aOther.myPermission == EPermission::Write);
}
