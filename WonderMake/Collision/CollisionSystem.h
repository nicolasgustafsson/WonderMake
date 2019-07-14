#pragma once

#include "Collision/Colliders.h"

#include "System/System.h"

#include "Utilities/plf_colony.h"

class CollisionFunctionality;

class CollisionSystem final
	: public System
{
public:
	CollisionSystem() noexcept;

	void Tick() noexcept override;

	template<typename TCallback>
	inline void Overlap(const Colliders::Shape& aCollider, const TCallback& aCallback) const noexcept;

	Colliders::Shape& CreateSphereCollider(CollisionFunctionality& aCollisionFunctionality, const SVector2f aPosition, const f32 aRadius);

	bool DestroyCollider(Colliders::Shape& aCollider);

private:
	[[nodiscard]] static bool TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept;
	[[nodiscard]] static bool TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept;

	[[nodiscard]] static bool TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept;

	plf::colony<Colliders::Shape> myColliders;
};

template<typename TCallback>
inline void CollisionSystem::Overlap(const Colliders::Shape& aCollider, const TCallback& aCallback) const noexcept
{
	for (const auto& collider : myColliders)
	{
		if (TestCollision(aCollider, collider))
		{
			aCallback(collider);
		}
	}
}
