#include "pch.h"

#include "CollisionFunctionality.h"
#include "CollisionSystem.h"
#include "ColliderDebug.h"

#include "System/SystemPtr.h"
#include "Debugging/DebugSettingsSystem.h"

CollisionFunctionality::CollisionFunctionality(Object& aOwner) noexcept
	: Super(aOwner), Debugged("Collision")
{

}

CollisionFunctionality::~CollisionFunctionality()
{
	auto& collisionComponent = Get<SCollisionComponent>();
	SystemPtr<CollisionSystem> collisionSystem;

	for (auto& collider : collisionComponent.Colliders)
		collisionSystem->DestroyCollider(*collider.Collider);

	collisionComponent.Colliders.clear();
}

void CollisionFunctionality::Tick()
{
	UpdateCollisionTransforms();

	if (!SystemPtr<DebugSettingsSystem>()->GetOrCreateDebugValue("Should draw colliders", true))
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

void CollisionFunctionality::Debug()
{
	auto& collisionComponent = Get<SCollisionComponent>();

	ImGui::Begin("Enemy Collision");

	for (auto& collider : collisionComponent.Colliders)
	{
		if (!collider.Collider)
		{
			continue;
		}

		ImGui::SliderFloat2("Offset", &collider.Offset.X, -1000.f, 1000.f);

		std::visit([](auto& aCollider)
			{
				using T = std::decay_t<decltype(aCollider)>;
				
				if constexpr (std::is_same_v<T, Colliders::SSphere>)
				{
					ImGui::SliderFloat("Radius", &aCollider.Radius, 0, 500);
				}
				else if constexpr (std::is_same_v<T, Colliders::SLine>)
				{
					ImGui::SliderFloat2("End Offset", &aCollider.EndOffsetFromPosition.X, 0, 500);
				}
				else
				{
					static_assert(std::false_type::value, "Collider not implemented!");
				}
			}, *collider.Collider);

		DrawCollider(*collider.Collider);
	}

	ImGui::End();
}

void CollisionFunctionality::UpdateCollisionTransforms()
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();

	const auto transformation = transformFunctionality.GetMatrix();
	const f32 rotation = transformFunctionality.GetRotation();

	for (auto& collider : collisionComponent.Colliders)
	{
		if (!collider.Collider)
			continue;

		std::visit([collider, transformation, rotation](auto& aCollider)
			{
				using T = std::decay_t<decltype(aCollider)>;

				aCollider.Position = collider.Offset * transformation;

				if constexpr (std::is_same_v<T, Colliders::SLine>)
				{
					aCollider.Rotation = rotation;
				}

			}, *collider.Collider);
	}
}
