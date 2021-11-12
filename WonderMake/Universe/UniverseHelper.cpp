#include "pch.h"
#include "UniverseHelper.h"

#include "Utilities/Id.h"

Id GetCurrentUniverseId()
{
	return SystemPtr<UniverseManagerSystem>()->GetCurrentUniverse();
}
