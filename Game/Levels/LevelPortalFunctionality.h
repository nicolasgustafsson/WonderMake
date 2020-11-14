#pragma once

#include "Functionalities/Functionality.h"

#include "Collision/CollisionFunctionality.h"

class LevelPortalFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionFunctionality, PWrite>>>
{
public:
	LevelPortalFunctionality();
	~LevelPortalFunctionality();

private:
	bool myDoOnce = false;
};
