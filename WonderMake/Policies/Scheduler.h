#pragma once

#include "Policies/Policy.h"
#include "Policies/SystemId.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Scheduler
{
public:
	void AddToSchedule(const SystemId aSystemId, std::vector<Policy> aPolicyList);
	[[nodiscard]] bool Schedule();

	void ResetQueue();
	[[nodiscard]] std::optional<SystemId> DequeueSystem();
	void MarkSystemAsDone(const SystemId aSystemId);
	[[nodiscard]] bool IsDone() const noexcept;

private:
	struct SchedulingInfo
	{
		SystemId mySystemId;
		std::vector<Policy>		myPolicies;
		std::vector<SystemId>	myDependsOnThis;
		std::vector<SystemId>	myThisDependsOn;
		std::vector<SystemId>	myCanRunParallelWith;
	};

	void ConstructItemDependencies(SchedulingInfo& aSystem);
	[[nodiscard]] bool TraverseDown(const SchedulingInfo& aRootSystem, const SchedulingInfo& aSystem, std::unordered_set<SystemId>& aOutList) const;
	[[nodiscard]] bool TraverseUp(const SchedulingInfo& aRootSystem, const SchedulingInfo& aSystem, std::unordered_set<SystemId>& aOutList) const;
	[[nodiscard]] bool SystemCanRunParallelWith(const SchedulingInfo& aSystem, const SchedulingInfo& aOtherSystem) const noexcept;
	[[nodiscard]] bool ValidateSystem(SchedulingInfo& aSystem);

	[[nodiscard]] bool CanItemRun(const SchedulingInfo& aSystem) const;
	void TryToAddToQueue(const SystemId aSystemId);

	std::unordered_map<SystemId, SchedulingInfo> mySystems;

	std::unordered_set<SystemId> myQueueTemplate;
	std::unordered_set<SystemId> myQueueReady;
	std::unordered_set<SystemId> mySystemRunning;
	std::unordered_set<SystemId> mySystemCompleted;
};
