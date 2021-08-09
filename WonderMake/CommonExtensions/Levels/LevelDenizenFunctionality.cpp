#include "pch.h"
#include "LevelDenizenFunctionality.h" 

REGISTER_COMPONENT(SLevelDenizenComponent);
REGISTER_FUNCTIONALITY(LevelDenizenFunctionality);

void LevelDenizenFunctionality::Destroy()
{
	Get<SLevelDenizenComponent>().SlatedForRemoval = true;
}

LevelFunctionality* LevelDenizenFunctionality::GetLevel() const
{
	return Get<SLevelDenizenComponent>().Level;
}
