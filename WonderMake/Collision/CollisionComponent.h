#pragma once

#include "Components/Component.h"

#include <vector>

namespace Colliders
{
	struct SBase;
}

struct SCollider
{
	SVector2f				Offset;
	Colliders::SBase*		Collider = nullptr;
};

struct SCollisionComponent final
	: public SComponent
{
	std::vector<SCollider>	Colliders;
};
