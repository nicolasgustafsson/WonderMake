#pragma once

#include "wondermake-utility/Typedefs.h"

#include "wondermake-base/SystemId.h"
#include "wondermake-base/SystemPolicy.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Scheduler
{
public:
	void AddToSchedule(const SystemId aSystemId, std::vector<Policy> aPolicyList);
	[[nodiscard]] bool Schedule();

	void AddSystem(const SystemId aSystemId);
	[[nodiscard]] std::optional<SystemId> DequeueSystem();
	void MarkSystemAsDone(const SystemId aSystemId);

private:

	struct SSchedulingInfo
	{
		SystemId						Id;
		std::vector<Policy>				Policies;

		std::unordered_set<SystemId>	DependencyHierarchy;
		std::unordered_set<SystemId>	DependsOnThis;
		std::unordered_set<SystemId>	ThisDependsOn;
		std::unordered_set<SystemId>	CanRunParallelWith;
	};

	struct SEnqueuedSystem
	{
		SystemId						Id;
		u32								Priority = 1;
	};

	[[nodiscard]] bool ConstructSystemDependencies(SSchedulingInfo& aSystemInfo);
	void ConstructSystemParallels(SSchedulingInfo& aSystemInfo);
	[[nodiscard]] bool ConstructDependencyHierarchy(SSchedulingInfo& aSystemInfo);

	std::unordered_map<SystemId, SSchedulingInfo>	mySystemsInfo;
	std::vector<SEnqueuedSystem>					myEnqueuedSystems;
	std::unordered_set<SystemId>					myRunningSystem;
};
