#pragma once

#include "Collision/Colliders.h"

#include "wondermake-base/Component.h"

#include <vector>

class CollisionSystem;

struct SCollider
{
	SVector2f				Offset;
	Colliders::Shape*		Collider = nullptr;
};

struct SCollisionComponent final
	: public STraitedComponent<
		STrait::Set<
			STWonderMake>>
{
	// Have to keep this here as the order of operation when destoying
	// systems is wrong, making it possible for collision functionalities
	// to be destroyed after the CollisionSystem.
	std::weak_ptr<CollisionSystem>	System;

	plf::colony<SCollider>	Colliders;
};
