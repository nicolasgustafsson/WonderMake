#include "pch.h"
#include "UniverseManagerSystem.h"

REGISTER_SYSTEM(UniverseManagerSystem);

UniverseStackGuard UniverseManagerSystem::PushUniverse(const std::string aUniverseName)
{
	myCurrentUniverseName = aUniverseName;
	myUniverseStack.Add(GetUniverseId(std::move(aUniverseName)));

	return UniverseStackGuard();
}

UniverseStackGuard::~UniverseStackGuard()
{
	SystemPtr<UniverseManagerSystem>()->PopUniverse();
}
