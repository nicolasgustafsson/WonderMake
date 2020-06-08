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
		BuffGiverFunctionality,
		Policy::Set<
			Policy::Add<SBuffGiverComponent, Policy::EPermission::Write>,
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>>>
{
public:
	BuffGiverFunctionality(Object& aOwner) noexcept;

	void Initialize(BuffBlueprint& aBuffBlueprint, const f32 aRadius);

	void Tick();
};

