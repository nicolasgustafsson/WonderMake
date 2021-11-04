#include "pch.h"
#include "UniverseDenizen.h"

REGISTER_COMPONENT(SUniverseDenizenComponent)
REGISTER_FUNCTIONALITY(UniverseDenizen)

void UniverseDenizen::SetUniverse(std::string aUniverseName)
{
	Get<SUniverseDenizenComponent>().myUniverseId = Get<UniverseSystem>().GetUniverseId(aUniverseName);
}

Id UniverseDenizen::GetUniverseId() const
{
	return Get<SUniverseDenizenComponent>().myUniverseId;
}
