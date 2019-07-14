#include "pch.h"

#include "CollisionSystem.h"

CollisionSystem::CollisionSystem() noexcept
{
	// EnableTick();
}

void CollisionSystem::Tick() noexcept
{

}

Colliders::Shape& CollisionSystem::CreateSphereCollider(CollisionFunctionality& aCollisionFunctionality, const SVector2f aPosition, const f32 aRadius)
{
	Colliders::SSphere collider;

	collider.CollisionFunctionality = &aCollisionFunctionality;
	collider.Position = aPosition;
	collider.Radius = aRadius;

	return (*myColliders.emplace(collider));
}

bool CollisionSystem::DestroyCollider(Colliders::Shape& aCollider)
{
	auto it = myColliders.get_iterator_from_pointer(&aCollider);
	
	if (it == myColliders.end())
	{
		return false;
	}

	myColliders.erase(it);

	return true;
}

bool CollisionSystem::TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept
{
	return std::visit([aColliderB](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereCollision(aCollider, aColliderB);
			}
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aColliderA);
}

bool CollisionSystem::TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aSphere](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereVsSphereCollision(aSphere, aCollider);
			}
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}

bool CollisionSystem::TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept
{
	const auto delta = aSphereA.Position - aSphereB.Position;
	
	return (delta.LengthSquared() <= std::powf(aSphereA.Radius + aSphereB.Radius, 2));
}
