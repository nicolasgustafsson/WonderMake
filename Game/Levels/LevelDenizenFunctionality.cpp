#include "pch.h"
#include "LevelDenizenFunctionality.h" 

REGISTER_COMPONENT(SLevelDenizenComponent);
REGISTER_FUNCTIONALITY(LevelDenizenFunctionality);

LevelDenizenFunctionality::LevelDenizenFunctionality(Object& aOwner, Dependencies&& aDependencies)
	: Super(aOwner, std::move(aDependencies))
{

}
