#include "CollisionFunctionality.h"
#include "CollisionSystem.h"
#include "ColliderDebug.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"

WM_REGISTER_COMPONENT(SCollisionComponent);
WM_REGISTER_FUNCTIONALITY(CollisionFunctionality);

CollisionFunctionality::~CollisionFunctionality()
{
	auto& collisionComponent = Get<SCollisionComponent>();

	for (auto& collider : collisionComponent.Colliders)
		Get<CollisionSystem>().DestroyCollider(*collider.Collider);

	collisionComponent.Colliders.clear();
}

void CollisionFunctionality::Tick()
{
	UpdateCollisionTransforms();

	if (!Get<DebugSettingsSystem>().GetOrCreateDebugValue("Should draw colliders", true))
		return;

	auto& collisionComponent = Get<SCollisionComponent>();

	for (auto& collider : collisionComponent.Colliders)
	{
		if (!collider.Collider)
		{
			continue;
		}
		DrawCollider(*collider.Collider);
	}
}

void CollisionFunctionality::UpdateCollisionTransforms()
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality2D>();

	const auto transformation = transformFunctionality.GetMatrix();
	const auto rotation = transformFunctionality.GetRotation();

	for (auto& collider : collisionComponent.Colliders)
	{
		if (!collider.Collider)
			continue;

		std::visit([collider, transformation, rotation](auto& aCollider)
			{
				using T = std::decay_t<decltype(aCollider)>;

				aCollider.Position = collider.Offset * transformation;

				if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
					aCollider.Rotation = rotation;

			}, *collider.Collider);
	}
}
