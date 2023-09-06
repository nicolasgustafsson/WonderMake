#pragma once

#include "wondermake-utility/Typedefs.h"

#include "wondermake-base/SystemPolicy.h"
#include "wondermake-base/SystemTypeId.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Scheduler
{
public:
	void AddToSchedule(const SystemTypeId aSystemId, std::vector<Policy> aPolicyList);
	[[nodiscard]] bool Schedule();

	void AddSystem(const SystemTypeId aSystemId);
	[[nodiscard]] std::optional<SystemTypeId> DequeueSystem();
	void MarkSystemAsDone(const SystemTypeId aSystemId);

private:

	struct SSchedulingInfo
	{
		SystemTypeId						Id;
		std::vector<Policy>				Policies;

		std::unordered_set<SystemTypeId>	DependencyHierarchy;
		std::unordered_set<SystemTypeId>	DependsOnThis;
		std::unordered_set<SystemTypeId>	ThisDependsOn;
		std::unordered_set<SystemTypeId>	CanRunParallelWith;
	};

	struct SEnqueuedSystem
	{
		SystemTypeId						Id;
		u32								Priority = 1;
	};

	[[nodiscard]] bool ConstructSystemDependencies(SSchedulingInfo& aSystemInfo);
	void ConstructSystemParallels(SSchedulingInfo& aSystemInfo);
	[[nodiscard]] bool ConstructDependencyHierarchy(SSchedulingInfo& aSystemInfo);

	std::unordered_map<SystemTypeId, SSchedulingInfo>	mySystemsInfo;
	std::vector<SEnqueuedSystem>					myEnqueuedSystems;
	std::unordered_set<SystemTypeId>					myRunningSystem;
};
