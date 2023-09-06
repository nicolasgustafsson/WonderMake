#include "wondermake-base/SystemCounter.h"

void SystemCounter::IncreaseGeneration()
{
	++myGeneration;
}

void SystemCounter::AddSystem(SystemId aId, std::string aTypeName)
{
	mySystems.emplace(std::make_pair(aId,
		SSystemInfo
		{
			.TypeName	= std::move(aTypeName),
			.Generation	= myGeneration
		}));
}

void SystemCounter::AddSystemDependency(SystemId aId, SystemId aDependencyId)
{
	auto it = mySystems.find(aId);

	if (it == mySystems.end())
		return;

	it->second.Dependencies.emplace(aDependencyId);
}

void SystemCounter::RemoveSystem(SystemId aId)
{
	mySystems.erase(aId);
}
