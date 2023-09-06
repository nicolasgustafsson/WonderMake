#pragma once

#include "wondermake-base/SystemId.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

class SystemCounter
{
public:
	struct SSystemInfo
	{
		std::string						TypeName;
		std::unordered_set<SystemId>	Dependencies;
		u32								Generation = 0;
	};

	void IncreaseGeneration();

	void AddSystem(SystemId aId, std::string aTypeName);
	void AddSystemDependency(SystemId aId, SystemId aDependencyId);
	void RemoveSystem(SystemId aId);

	[[nodiscard]] inline const auto& Systems() const noexcept
	{
		return mySystems;
	}

private:
	std::unordered_map<SystemId, SSystemInfo>	mySystems;
	u32											myGeneration = 0;

};
