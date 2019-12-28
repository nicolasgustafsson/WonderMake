#pragma once

#include "Collision/CollisionComponent.h"

#include "Utilities/Debugging/Debugged.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Collision/CollisionSystem.h"

namespace Colliders
{
	struct SSphere;
}

class CollisionFunctionality final
	: public Functionality<
		CollisionFunctionality,
		SCollisionComponent,
		TransformFunctionality>
	, public Debugged
{
public:

	CollisionFunctionality(Object& aOwner) noexcept;
	~CollisionFunctionality();

	void Tick();

	template<typename TIdentifyingFunctionality>
	SCollider& AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius);

	template<typename TIdentifyingFunctionality>
	SCollider& AddLineCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aSecondOffset);

	template<typename TFunctionalityToReactAgainst>
	void AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactAgainst&)> aCallback);

	void Debug() override;
};

template<typename TIdentifyingFunctionality>
SCollider& CollisionFunctionality::AddLineCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aSecondOffset)
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	SystemPtr<CollisionSystem> collisionSystem;

	const auto transformation = transformFunctionality.GetMatrix();

	SCollider collider;

	collider.Collider = &collisionSystem->CreateLineCollider(aFunctionalityIdentifier, aOffset * transformation, aSecondOffset * transformation);
	collider.Offset = aOffset;

	return *collisionComponent.Colliders.emplace(collider);
}

template<typename TFunctionalityToReactAgainst>
void CollisionFunctionality::AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactAgainst&)> aCallback)
{
	SystemPtr<CollisionSystem> collisionSystem;

	collisionSystem->AddReaction<TFunctionalityToReactAgainst>(*aCollider.Collider, aCallback);
}

template<typename TIdentifyingFunctionality>
SCollider& CollisionFunctionality::AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius)
{
	SCollisionComponent& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	SystemPtr<CollisionSystem> collisionSystem;

	const auto transformation = transformFunctionality.GetMatrix();

	SCollider collider;

	collider.Collider = &collisionSystem->CreateSphereCollider(aFunctionalityIdentifier, aOffset * transformation, aRadius);
	collider.Offset = aOffset;

	return *collisionComponent.Colliders.emplace(collider);
}
