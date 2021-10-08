#pragma once
#include "Functionalities/Functionality.h"

class ObjectContainerFunctionality;

struct SSubobjectComponent : public SComponent
{
	ObjectContainerFunctionality* Level = nullptr;

	bool PersistentOnLevelChange = false;
	bool SlatedForRemoval = false;
};

class SubobjectFunctionality : public Functionality<SSubobjectComponent>
{
public:
	void Destroy();

	ObjectContainerFunctionality* GetLevel() const;
};
