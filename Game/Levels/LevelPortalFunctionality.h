#pragma once

#include "Functionalities/Functionality.h"

#include "Collision/CollisionFunctionality.h"

class LevelPortalFunctionality
	: public Functionality<
		LevelPortalFunctionality,
		Policy::Set<
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>>>
{
public:
	LevelPortalFunctionality(Object& aOwner);
	~LevelPortalFunctionality();

private:
	bool myDoOnce = false;
};

REGISTER_FUNCTIONALITY(LevelPortalFunctionality);
