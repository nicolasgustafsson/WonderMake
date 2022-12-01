#pragma once

#include "Collision/CollisionComponent.h"

#include "Utilities/Debugging/Debugged.h"

#include "Functionalities/TransformFunctionality.h"
#include "Collision/CollisionSystem.h"

#include "wondermake-base/Functionality.h"

class DebugSettingsSystem;

namespace Colliders
{
	struct SSphere;
}

class CollisionFunctionality final
	: public Functionality<
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<DebugSettingsSystem, PWrite>,
			PAdd<SCollisionComponent, PWrite>,
			PAdd<TransformFunctionality2D, PRead>>>
	, public Debugged
{
public:

	CollisionFunctionality();
	~CollisionFunctionality();

	void Tick();

	void UpdateCollisionTransforms();

	template<typename TIdentifyingFunctionality>
	SCollider& AddSphereCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const f32 aRadius)
	{
		SCollisionComponent& collisionComponent = Get<SCollisionComponent>();
		const auto& transformFunctionality = Get<TransformFunctionality2D>();
		auto& collisionSystem = Get<CollisionSystem>();

		const auto transformation = transformFunctionality.GetMatrix();

		SVector2f position = aOffset * transformation;

		SCollider collider;

		collider.Collider = &collisionSystem.CreateSphereCollider(aFunctionalityIdentifier, position, aRadius);
		collider.Offset = aOffset;

		return *collisionComponent.Colliders.emplace(collider);
	}
	template<typename TIdentifyingFunctionality>
	SCollider& AddAABBCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aDimension)
	{
		SCollisionComponent& collisionComponent = Get<SCollisionComponent>();
		const auto& transformFunctionality = Get<TransformFunctionality2D>();
		auto& collisionSystem = Get<CollisionSystem>();

		const auto transformation = transformFunctionality.GetMatrix();

		SVector2f position = aOffset * transformation;

		SCollider collider;

		collider.Collider = &collisionSystem.CreateAABBCollider(aFunctionalityIdentifier, position, aDimension);
		collider.Offset = aOffset;

		return *collisionComponent.Colliders.emplace(collider);
	}
	template<typename TIdentifyingFunctionality>
	SCollider& AddLineCollider(TIdentifyingFunctionality& aFunctionalityIdentifier, const SVector2f aOffset, const SVector2f aSecondOffset)
	{
		auto& collisionComponent = Get<SCollisionComponent>();
		const auto& transformFunctionality = Get<TransformFunctionality2D>();
		auto& collisionSystem = Get<CollisionSystem>();

		const auto transformation = transformFunctionality.GetMatrix();

		SCollider collider;

		collider.Collider = &collisionSystem.CreateLineCollider(aFunctionalityIdentifier, aOffset * transformation, aSecondOffset * transformation);
		collider.Offset = aOffset;

		return *collisionComponent.Colliders.emplace(collider);
	}

	template<typename TFunctionalityToReactAgainst>
	void AddReaction(SCollider& aCollider, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		Get<CollisionSystem>().AddReaction<TFunctionalityToReactAgainst>(*aCollider.Collider, aCallback);
	}

	void Debug() override;
};
