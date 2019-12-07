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

	template<typename TFunctionalityToReactTo>
	void AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactTo&)> aCallback);

	void Debug() override;

private:
};

template<typename TFunctionalityToReactTo>
void CollisionFunctionality::AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactTo&)> aCallback)
{
	SystemPtr<CollisionSystem> collisionSystem;

	collisionSystem->AddReaction<TFunctionalityToReactTo>(*aCollider.Collider, aCallback);
}

template<typename TIdentifyingFunctionality>
SCollider& CollisionFunctionality::AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius)
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	SystemPtr<CollisionSystem> collisionSystem;

	const auto transformation = transformFunctionality.GetMatrix();

	SCollider collider;

	collider.Collider = &collisionSystem->CreateSphereCollider(aFunctionalityIdentifier, aOffset * transformation, aRadius);
	collider.Offset = aOffset;

	collisionComponent.Colliders.emplace_back(collider);
	return collisionComponent.Colliders[collisionComponent.Colliders.size() - 1];
}
