#include "pch.h"

#include "Policies/Scheduler.h"

void Scheduler::AddToSchedule(const SystemId aSystemId, std::vector<Policy> aPolicyList)
{
	SchedulingInfo item = {
		aSystemId,
		std::move(aPolicyList)
	};
	mySystems.emplace(std::make_pair(aSystemId, std::move(item)));
}

[[nodiscard]] bool Scheduler::Schedule()
{
	myQueueTemplate.clear();

	for (auto& systemIt : mySystems)
	{
		systemIt.second.myDependsOnThis.clear();
		systemIt.second.myThisDependsOn.clear();
		systemIt.second.myCanRunParallelWith.clear();
	}

	for (auto& systemIt : mySystems)
	{
		ConstructItemDependencies(systemIt.second);
	}

	for (auto& systemIt : mySystems)
	{
		if (systemIt.second.myThisDependsOn.empty())
		{
			myQueueTemplate.insert(systemIt.second.mySystemId);
		}

		if (!ValidateSystem(systemIt.second))
		{
			return false;
		}
	}

	return true;
}

void Scheduler::ResetQueue()
{
	myQueueReady = myQueueTemplate;
	mySystemRunning.clear();
	mySystemCompleted.clear();
}

[[nodiscard]] std::optional<SystemId> Scheduler::DequeueSystem()
{
	if (myQueueReady.empty())
	{
		return std::optional<SystemId>();
	}

	for (const auto systemId : myQueueReady)
	{
		const auto itemIt = mySystems.find(systemId);
		if (itemIt == mySystems.end())
		{
			// TODO(Kevin): Assert, something has gone terribly wrong.
			WmLog(TagError, "Unknown system id found.");
			return std::optional<SystemId>();
		}
		
		const auto& item = itemIt->second;
		if (!CanItemRun(item))
		{
			continue;
		}

		myQueueReady.erase(systemId);
		mySystemRunning.insert(systemId);
		return systemId;
	}

	return std::optional<SystemId>();
}

void Scheduler::MarkSystemAsDone(const SystemId aSystemId)
{
	mySystemRunning.erase(aSystemId);
	mySystemCompleted.insert(aSystemId);

	const auto itemIt = mySystems.find(aSystemId);
	if (itemIt == mySystems.end())
	{
		// TODO(Kevin): Assert, something has gone terribly wrong.
		WmLog(TagError, "Unknown system id found.");
		return;
	}

	const auto& item = itemIt->second;
	for (const auto dependencyId : item.myDependsOnThis)
	{
		TryToAddToQueue(dependencyId);
	}
}

[[nodiscard]] bool Scheduler::IsDone() const noexcept
{
	return myQueueReady.empty()
		&& mySystemRunning.empty();
}

void Scheduler::ConstructItemDependencies(SchedulingInfo& aSystem)
{
	for (const auto policy : aSystem.myPolicies)
	{
		const auto it = mySystems.find(policy.myDependencyId);
		if (it == mySystems.cend())
		{
			continue;
		}

		it->second.myThisDependsOn.emplace_back(aSystem.mySystemId);
		aSystem.myDependsOnThis.emplace_back(policy.myDependencyId);
	}
}


[[nodiscard]] bool Scheduler::TraverseDown(const SchedulingInfo& aRootSystem, const SchedulingInfo& aSystem, std::unordered_set<SystemId>& aOutList) const
{
	for (const auto policy : aSystem.myPolicies)
	{
		if (policy.myDependencyId == aRootSystem.mySystemId)
		{
			WmLog(TagError, "Circular System dependency!");
			return false;
		}

		const auto it = mySystems.find(policy.myDependencyId);
		if (it == mySystems.cend())
		{
			continue;
		}

		aOutList.insert(policy.myDependencyId);
		if (!TraverseDown(aRootSystem, it->second, aOutList))
		{
			return false;
		}
	}
	return true;
}


[[nodiscard]] bool Scheduler::TraverseUp(const SchedulingInfo& aRootSystem, const SchedulingInfo& aSystem, std::unordered_set<SystemId>& aOutList) const
{
	for (const auto policy : aSystem.myPolicies)
	{
		if (policy.myDependencyId == aRootSystem.mySystemId)
		{
			WmLog(TagError, "Circular System dependency!");
			return false;
		}

		const auto it = mySystems.find(policy.myDependencyId);
		if (it == mySystems.cend())
		{
			continue;
		}

		aOutList.insert(policy.myDependencyId);
		if (!TraverseUp(aRootSystem, it->second, aOutList))
		{
			return false;
		}
	}
	return true;
}

[[nodiscard]] bool Scheduler::SystemCanRunParallelWith(const SchedulingInfo& aSystem, const SchedulingInfo& aOtherSystem) const noexcept
{
	for (const auto& policy : aSystem.myPolicies)
	{
		for (const auto& otherPolicy : aOtherSystem.myPolicies)
		{
			if (policy.Conflicts(otherPolicy))
			{
				return false;
			}
		}
	}
	return true;
}

[[nodiscard]] bool Scheduler::ValidateSystem(SchedulingInfo& aSystem)
{
	std::unordered_set<SystemId> dependencySet;

	if (!TraverseDown(aSystem, aSystem, dependencySet)
		&& !TraverseUp(aSystem, aSystem, dependencySet))
	{
		return false;
	}

	for (auto& systemIt : mySystems)
	{
		if (systemIt.second.mySystemId == aSystem.mySystemId
			|| dependencySet.find(systemIt.second.mySystemId) != dependencySet.end()
			|| std::find(aSystem.myCanRunParallelWith.begin(), aSystem.myCanRunParallelWith.end(), systemIt.second.mySystemId) != aSystem.myCanRunParallelWith.end()
			|| !SystemCanRunParallelWith(aSystem, systemIt.second))
		{
			continue;
		}

		aSystem.myCanRunParallelWith.emplace_back(systemIt.second.mySystemId);
		systemIt.second.myCanRunParallelWith.emplace_back(aSystem.mySystemId);
	}

	return true;
}

[[nodiscard]] bool Scheduler::CanItemRun(const SchedulingInfo& aSystem) const
{
	for (const auto runningSystem : mySystemRunning)
	{
		if (std::find(aSystem.myCanRunParallelWith.begin(), aSystem.myCanRunParallelWith.end(), runningSystem) == aSystem.myCanRunParallelWith.end())
		{
			return false;
		}
	}
	return true;
}

void Scheduler::TryToAddToQueue(const SystemId aSystemId)
{
	const auto itemIt = mySystems.find(aSystemId);
	if (itemIt == mySystems.end())
	{
		// TODO(Kevin): Assert, something has gone terribly wrong.
		WmLog(TagError, "Unknown system id found.");
		return;
	}

	const auto& item = itemIt->second;
	for (const auto dependencyId : item.myThisDependsOn)
	{
		if (mySystemCompleted.find(dependencyId) == mySystemCompleted.end())
		{
			return;
		}
	}

	myQueueReady.insert(aSystemId);
}
