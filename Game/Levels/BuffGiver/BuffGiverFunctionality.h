#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Levels/BuffGiver/BuffGiverRenderObject.h"

class BuffBlueprint;
struct SCollider;

struct SBuffGiverComponent : public SComponent
{ 
	BuffBlueprint* BuffBlueprint = nullptr;
	SCollider* Collider = nullptr;

	std::optional<BuffGiverRenderObject> RenderObject;
};

class BuffGiverFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<SBuffGiverComponent, PWrite>,
			PAdd<CollisionFunctionality, PWrite>,
			PAdd<TransformFunctionality, PWrite>>>
{
public:
	void Initialize(BuffBlueprint& aBuffBlueprint, const f32 aRadius);

	void Tick();
};
