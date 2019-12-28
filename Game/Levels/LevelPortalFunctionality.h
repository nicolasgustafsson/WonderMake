#pragma once

#include "Functionalities/Functionality.h"

#include "Collision/CollisionFunctionality.h"

class LevelPortalFunctionality
	: public Functionality<
		LevelPortalFunctionality,
		CollisionFunctionality>
{
public:
	LevelPortalFunctionality(Object& aOwner);
	~LevelPortalFunctionality();

private:
	bool myDoOnce = false;
};
