#pragma once

#include "Collision/CollisionComponent.h"

#include "Utilities/Debugging/Debugged.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Collision/CollisionSystem.h"

class DebugSettingsSystem;

namespace Colliders
{
	struct SSphere;
}

class CollisionFunctionality final
	: public Functionality<
		CollisionFunctionality,
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<DebugSettingsSystem, PWrite>,
			PAdd<SCollisionComponent, PWrite>,
			PAdd<TransformFunctionality, PRead>>>
	, public Debugged
{
public:

	CollisionFunctionality(Object& aOwner, Dependencies&& aDependencies) noexcept;
	~CollisionFunctionality();

	void Tick();

	void UpdateCollisionTransforms();

	template<typename TIdentifyingFunctionality>
	SCollider& AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius);

	template<typename TIdentifyingFunctionality>
	SCollider& AddLineCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aSecondOffset);

	template<typename TFunctionalityToReactAgainst>
	void AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback);

	void Debug() override;
};

template<typename TIdentifyingFunctionality>
SCollider& CollisionFunctionality::AddLineCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aSecondOffset)
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	auto& collisionSystem = Get<CollisionSystem>();

	const auto transformation = transformFunctionality.GetMatrix();

	SCollider collider;

	collider.Collider = &collisionSystem.CreateLineCollider(aFunctionalityIdentifier, aOffset * transformation, aSecondOffset * transformation);
	collider.Offset = aOffset;

	return *collisionComponent.Colliders.emplace(collider);
}

template<typename TFunctionalityToReactAgainst>
void CollisionFunctionality::AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
{
	Get<CollisionSystem>().AddReaction<TFunctionalityToReactAgainst>(*aCollider.Collider, aCallback);
}

template<typename TIdentifyingFunctionality>
SCollider& CollisionFunctionality::AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius)
{
	SCollisionComponent& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	auto& collisionSystem = Get<CollisionSystem>();

	const auto transformation = transformFunctionality.GetMatrix();

	SCollider collider;

	collider.Collider = &collisionSystem.CreateSphereCollider(aFunctionalityIdentifier, aOffset * transformation, aRadius);
	collider.Offset = aOffset;

	return *collisionComponent.Colliders.emplace(collider);
}
