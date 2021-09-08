#pragma once
#include "Functionalities/Functionality.h"

class LevelFunctionality;

struct SLevelDenizenComponent : public SComponent
{
	LevelFunctionality* Level = nullptr;

	bool PersistentOnLevelChange = false;
	bool SlatedForRemoval = false;
};

class LevelDenizenFunctionality : public Functionality<SLevelDenizenComponent>
{
public:
	void Destroy();

	LevelFunctionality* GetLevel() const;
};
