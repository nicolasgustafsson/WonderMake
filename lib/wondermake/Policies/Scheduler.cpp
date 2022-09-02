#include "pch.h"

#include "Policies/Scheduler.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <algorithm>

void Scheduler::AddToSchedule(const SystemId aSystemId, std::vector<Policy> aPolicyList)
{
	SSchedulingInfo item = {
		aSystemId,
		std::move(aPolicyList)
	};

	mySystemsInfo.emplace(std::make_pair(aSystemId, std::move(item)));
}

[[nodiscard]] bool Scheduler::Schedule()
{
	for (auto& systemIt : mySystemsInfo)
	{
		if (!ConstructSystemDependencies(systemIt.second))
		{
			return false;
		}
	}

	for (auto& systemInfoPair : mySystemsInfo)
	{
		auto& systemInfo = systemInfoPair.second;

		if (!ConstructDependencyHierarchy(systemInfo))
		{
			return false;
		}

		ConstructSystemParallels(systemInfo);
	}

	return true;
}

void Scheduler::AddSystem(const SystemId aSystemId)
{
	myEnqueuedSystems.push_back({ aSystemId });
}

[[nodiscard]] std::optional<SystemId> Scheduler::DequeueSystem()
{
	const auto canSystemRun = [&runningSystems = myRunningSystem](const auto& aSystemInfo)
	{
		for (const auto runningSystem : runningSystems)
		{
			if (aSystemInfo.CanRunParallelWith.find(runningSystem) == aSystemInfo.CanRunParallelWith.cend())
			{
				return false;
			}
		}
		return true;
	};
	const auto calculateAveragePriority = [&enqueuedSystems = myEnqueuedSystems](const auto& aSystemInfo) -> u32
	{
		u32 dependencyCount = 0;
		u32 priority = 1;

		for (const auto& system : enqueuedSystems)
		{
			if (aSystemInfo.DependencyHierarchy.find(system.Id) == aSystemInfo.DependencyHierarchy.cend())
			{
				continue;
			}

			++dependencyCount;
			priority += system.Priority;
		}

		if (dependencyCount == 0)
		{
			return 1;
		}

		return priority / dependencyCount;
	};

	const auto dequeuedIt = std::find_if(myEnqueuedSystems.begin(), myEnqueuedSystems.end(), [&systemsInfo = mySystemsInfo, &canSystemRun, &calculateAveragePriority](const auto& aSystem)
		{
			const auto systemIt = systemsInfo.find(aSystem.Id);
			if (systemIt == systemsInfo.cend())
			{
				// TODO(Kevin): Assert, something has gone terribly wrong.
				WmLogError(TagWonderMake << TagWmScheduler << "Unknown system id found.");

				return false;
			}

			const auto& systemInfo = systemIt->second;

			if (!canSystemRun(systemInfo))
			{
				return false;
			}

			const auto magicWeightValueWhichIsTotallyLegit = 2;
			const auto averagePriority = calculateAveragePriority(systemInfo);

			return aSystem.Priority * magicWeightValueWhichIsTotallyLegit > averagePriority;
		});

	if (dequeuedIt == myEnqueuedSystems.cend())
	{
		return std::optional<SystemId>();
	}

	const auto dequeuedId = dequeuedIt->Id;

	const auto systemIt = mySystemsInfo.find(dequeuedId);
	if (systemIt == mySystemsInfo.cend())
	{
		// TODO(Kevin): Assert, something has gone terribly wrong.
		WmLogError(TagWonderMake << TagWmScheduler << "Unknown system id found.");

		return std::optional<SystemId>();
	}

	myRunningSystem.emplace(dequeuedId);
	myEnqueuedSystems.erase(dequeuedIt);

	const auto& systemInfo = systemIt->second;

	for (auto& system : myEnqueuedSystems)
	{
		if (systemInfo.DependencyHierarchy.find(system.Id) == systemInfo.DependencyHierarchy.cend())
		{
			continue;
		}

		++system.Priority;
	}

	return dequeuedId;
}

void Scheduler::MarkSystemAsDone(const SystemId aSystemId)
{
	myRunningSystem.erase(aSystemId);
}

[[nodiscard]] bool Scheduler::ConstructSystemDependencies(SSchedulingInfo& aSystemInfo)
{
	for (const auto policy : aSystemInfo.Policies)
	{
		const auto it = mySystemsInfo.find(policy.myDependencyId);
		if (it == mySystemsInfo.cend())
		{
			// TODO(Kevin): Assert, something has gone terribly wrong.
			WmLogError(TagWonderMake << TagWmScheduler << "Unknown system id found.");

			return false;
		}

		it->second.DependsOnThis.emplace(aSystemInfo.Id);
		aSystemInfo.ThisDependsOn.emplace(policy.myDependencyId);
	}

	return true;
}

void Scheduler::ConstructSystemParallels(SSchedulingInfo& aSystemInfo)
{
	const auto canRunParallelWith = [](const auto& aSystemInfo, const auto& aOtherSystem)
	{
		for (const auto& policy : aSystemInfo.Policies)
		{
			for (const auto& otherPolicy : aOtherSystem.Policies)
			{
				if (policy.Conflicts(otherPolicy))
				{
					return false;
				}
			}
		}
		return true;
	};

	for (auto& systemIt : mySystemsInfo)
	{
		if (systemIt.second.Id == aSystemInfo.Id
			|| aSystemInfo.DependencyHierarchy.find(systemIt.second.Id) != aSystemInfo.DependencyHierarchy.end()
			|| aSystemInfo.CanRunParallelWith.find(systemIt.second.Id) != aSystemInfo.CanRunParallelWith.cend()
			|| !canRunParallelWith(aSystemInfo, systemIt.second))
		{
			continue;
		}

		aSystemInfo.CanRunParallelWith.emplace(systemIt.second.Id);
		systemIt.second.CanRunParallelWith.emplace(aSystemInfo.Id);
	}
}

[[nodiscard]] bool Scheduler::ConstructDependencyHierarchy(SSchedulingInfo& aSystemInfo)
{
	enum class EDirection
	{
		Up,
		Down
	};

	const auto traverse = [&rootSystemInfo = aSystemInfo, &systems = mySystemsInfo](const auto aTraverseFunc, const EDirection aDirection, const auto& aSystem) -> bool
	{
		auto& dependencyList = aDirection == EDirection::Up ? aSystem.ThisDependsOn : aSystem.DependsOnThis;

		for (const auto dependencyId : dependencyList)
		{
			if (dependencyId == rootSystemInfo.Id)
			{
				WmLogError(TagWonderMake << TagWmScheduler << "Circular System dependency.");
				return false;
			}

			const auto it = systems.find(dependencyId);
			if (it == systems.cend())
			{
				continue;
			}

			rootSystemInfo.DependencyHierarchy.insert(dependencyId);
			if (!aTraverseFunc(aTraverseFunc, aDirection, it->second))
			{
				return false;
			}
		}
		return true;
	};

	if (!traverse(traverse, EDirection::Down, aSystemInfo)
		|| !traverse(traverse, EDirection::Up, aSystemInfo))
	{
		return false;
	}

	return true;
}
