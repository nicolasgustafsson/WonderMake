#include "pch.h"

#include "CollisionFunctionality.h"
#include "CollisionSystem.h"
#include "ColliderDebug.h"

#include "System/SystemPtr.h"

CollisionFunctionality::CollisionFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{

}

CollisionFunctionality::~CollisionFunctionality()
{
	auto& collisionComponent = Get<SCollisionComponent>();
	SystemPtr<CollisionSystem> collisionSystem;

	for (auto& collider : collisionComponent.Colliders)
	{
		collisionSystem->DestroyCollider(*collider.Collider);
	}

	collisionComponent.Colliders.clear();
}

void CollisionFunctionality::Tick()
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();

	const auto tranformation = transformFunctionality.GetMatrix();

	for (auto& collider : collisionComponent.Colliders)
	{
		collider.Collider->Position = collider.Offset * tranformation;
	}
}

void CollisionFunctionality::AddSphereCollider(const SVector2f aOffset, const f32 aRadius)
{
	auto& collisionComponent = Get<SCollisionComponent>();
	const auto& transformFunctionality = Get<TransformFunctionality>();
	SystemPtr<CollisionSystem> collisionSystem;

	const auto tranformation = transformFunctionality.GetMatrix();

	SCollider collider;
	
	collider.Collider = &collisionSystem->CreateSphereCollider(*this, aOffset * tranformation, aRadius);
	collider.Offset = aOffset;

	collisionComponent.Colliders.emplace_back(collider);
}

void CollisionFunctionality::Debug()
{
	auto& collisionComponent = Get<SCollisionComponent>();

	ImGui::Begin("Enemy Collision");

	for (auto& collider : collisionComponent.Colliders)
	{
		Colliders::SSphere& sphere = static_cast<Colliders::SSphere&>(*collider.Collider);

		DrawCollider(sphere);

		ImGui::SliderFloat2("Offset", &collider.Offset.X, -1000.f, 1000.f);
		ImGui::SliderFloat("Radius", &sphere.Radius, 0, 500);
	}

	ImGui::End();
}
