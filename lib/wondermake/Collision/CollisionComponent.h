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
	: public SComponent
{
	plf::colony<SCollider>	Colliders;
};
