#pragma once
#include "Functionalities/Functionality.h"

class LevelFunctionality;

struct SLevelDenizenComponent : public SComponent
{
	LevelFunctionality* Level = nullptr;

	bool PersistentOnLevelChange = false;
	bool SlatedForRemoval = false;
};

class LevelDenizenFunctionality : public Functionality<
	LevelDenizenFunctionality,
	Policy::Set<Policy::Add<SLevelDenizenComponent, Policy::EPermission::Write>>>
{
public:
	LevelDenizenFunctionality(Object& aOwner, Dependencies&& aDependencies);
};
