#include "pch.h"

#include "CollisionSystem.h"

CollisionSystem::CollisionSystem() noexcept
{
	// EnableTick();
}

void CollisionSystem::Tick() noexcept
{

}

Colliders::SSphere& CollisionSystem::CreateSphereCollider(CollisionFunctionality& aCollisionFunctionality, const SVector2f aPosition, const f32 aRadius)
{
	Colliders::SSphere collider;

	collider.CollisionFunctionality = &aCollisionFunctionality;
	collider.Position = aPosition;
	collider.Radius = aRadius;

	return (*mySphereColliders.emplace(collider));
}

bool CollisionSystem::DestroyCollider(Colliders::SBase& aCollider)
{
	switch(aCollider.Type)
	{
	case Colliders::Type::Sphere:
	{
		const auto it = mySphereColliders.get_iterator_from_pointer(static_cast<Colliders::SSphere*>(&aCollider));

		if (it == mySphereColliders.cend())
		{
			return false;
			
		}
		
		mySphereColliders.erase(it);
		
		return true;
	}
	}

	return false;
}

bool CollisionSystem::TestCollision(const Colliders::SBase& aColliderA, const Colliders::SBase& aColliderB) noexcept
{
	switch (aColliderA.Type)
	{
	case Colliders::Type::Sphere:
		return TestSphereCollision(static_cast<const Colliders::SSphere&>(aColliderA), aColliderB);
	}

	assert(false);

	return false;
}

bool CollisionSystem::TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::SBase& aCollider) noexcept
{
	switch (aCollider.Type)
	{
	case Colliders::Type::Sphere:
		return TestSphereVsSphereCollision(aSphere, static_cast<const Colliders::SSphere&>(aCollider));
	}

	assert(false);

	return false;
}

bool CollisionSystem::TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept
{
	const auto delta = aSphereA.Position - aSphereB.Position;
	
	return (delta.LengthSquared() <= std::powf(aSphereA.Radius + aSphereB.Radius, 2));
}
