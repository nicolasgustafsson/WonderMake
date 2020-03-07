#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"

class BuffBlueprint;
struct SCollider;

struct SBuffGiverComponent : public SComponent
{ 
	BuffBlueprint* BuffBlueprint = nullptr;
	SCollider* Collider = nullptr;
};

class BuffGiverFunctionality
	: public Functionality<BuffGiverFunctionality, SBuffGiverComponent, CollisionFunctionality>
{
public:
	BuffGiverFunctionality(Object& aOwner) noexcept;

	void Initialize(BuffBlueprint& aBuffBlueprint, const f32 aRadius);
};

