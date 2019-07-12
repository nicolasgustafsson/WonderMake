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
	inline void Overlap(const Colliders::SSphere& aCollider, const TCallback& aCallback) const noexcept;

	Colliders::SSphere& CreateSphereCollider(CollisionFunctionality& aCollisionFunctionality, const SVector2f aPosition, const f32 aRadius);

	bool DestroyCollider(Colliders::SBase& aCollider);

private:
	[[nodiscard]] static bool TestCollision(const Colliders::SBase& aColliderA, const Colliders::SBase& aColliderB) noexcept;
	[[nodiscard]] static bool TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::SBase& aCollider) noexcept;

	[[nodiscard]] static bool TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept;

	plf::colony<Colliders::SSphere> mySphereColliders;
};

template<typename TCallback>
inline void CollisionSystem::Overlap(const Colliders::SSphere& aCollider, const TCallback& aCallback) const noexcept
{
	for (const auto& collider : mySphereColliders)
	{
		if (TestSphereVsSphereCollision(aCollider, collider))
		{
			aCallback(collider);
		}
	}
}
