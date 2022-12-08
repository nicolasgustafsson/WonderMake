#pragma once

#include "Collision/Colliders.h"

#include "wondermake-base/Component.h"

#include <vector>

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
	plf::colony<SCollider>	Colliders;
};
