#pragma once

#include "Collision/Colliders.h"

#include "Components/Component.h"

#include <vector>

struct SColliderSerialization
{
	SVector2f				Position;
	f32						Radius = 0.f;
};

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

REGISTER_COMPONENT(SCollisionComponent);
