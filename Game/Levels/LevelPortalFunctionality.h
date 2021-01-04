#pragma once

#include "Functionalities/Functionality.h"

#include "Collision/CollisionFunctionality.h"

class JobSystem;

class LevelPortalFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionFunctionality, PWrite>,
			PAdd<JobSystem, PWrite>>>
{
public:
	LevelPortalFunctionality();

private:
	bool myDoOnce = false;
};
