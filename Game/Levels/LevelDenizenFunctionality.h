#pragma once
#include "Functionalities/Functionality.h"

class LevelFunctionality;

struct SLevelDenizenComponent : public SComponent
{
	LevelFunctionality* Level = nullptr;
};

class LevelDenizenFunctionality : public Functionality<
	LevelDenizenFunctionality,
	Policy::Set<Policy::Add<SLevelDenizenComponent, Policy::EPermission::Write>>>
{
public:
	LevelDenizenFunctionality(Object& aOwner);
};

