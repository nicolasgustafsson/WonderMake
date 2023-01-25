#pragma once

#include "Collision/Colliders.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/plf_colony.h"

#include <functional>

class CollisionFunctionality;
class ScheduleSystem;

 // TODO: Collision needs a rewrite to take policies into account.

class CollisionSystem final
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	CollisionSystem() noexcept;

	void Tick();

	template<typename TIdentifyingFunctionality>
	inline [[nodiscard]] Colliders::Shape& CreateSphereCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const f32 aRadius)
	{
		Colliders::SSphere collider;

		collider.Functionality	= &aFunctionality;
		collider.Position		= aPosition;
		collider.Radius			= aRadius;
		collider.Identifier		= typeid(TIdentifyingFunctionality).hash_code();

		return (*myCollidersByType[collider.Identifier].emplace(collider));
	}
	template<typename TIdentifyingFunctionality>
	inline [[nodiscard]] Colliders::Shape& CreateAABBCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const SVector2f aDimension)
	{
		Colliders::SAxisAlignedBoundingBox collider;

		collider.Functionality = &aFunctionality;
		collider.Position = aPosition;
		collider.Dimensions = aDimension;
		collider.Identifier = typeid(TIdentifyingFunctionality).hash_code();

		return (*myCollidersByType[collider.Identifier].emplace(collider));
	}
	template<typename TIdentifyingFunctionality>
	inline [[nodiscard]] Colliders::Shape& CreateLineCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const SVector2f aSecondPosition)
	{
		Colliders::SCollisionLine collider;

		collider.Functionality			= &aFunctionality;
		collider.Position				= aPosition;
		collider.EndOffsetFromPosition	= aSecondPosition - aPosition;
		collider.Identifier				= typeid(TIdentifyingFunctionality).hash_code();

		return (*myCollidersByType[collider.Identifier].emplace(collider));
	}

	template<typename TFunctionalityToReactAgainst>
	void AddReaction(Colliders::Shape& aShape, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		myCollidersWithReactions.insert(&aShape);

		std::visit([&](auto&& aShape)
			{
				aShape.Reactions.push_back(Colliders::SReaction(aCallback));
			}, aShape);
	}

	bool DestroyCollider(Colliders::Shape& aCollider);

	template<typename TFunctionalityToReactAgainst>
	inline void OverlapAgainstFunctionality(const Colliders::Shape& aCollider, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		Colliders::SReaction reaction(aCallback);

		OverlapAgainstFunctionalityInternal(aCollider, reaction);
	}

	template<typename TFunctionalityToReactAgainst>
	inline void OverlapPointAgainstFunctionality(const SVector2f aPosition, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		Colliders::SSphere collider;

		collider.Position = aPosition;
		collider.Radius = 0;

		OverlapAgainstFunctionality(collider, aCallback);
	}
	template<typename TFunctionalityToReactAgainst>
	inline void OverlapSphereAgainstFunctionality(const SVector2f aPosition, const f32 aRadius, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		Colliders::SSphere collider;

		collider.Position = aPosition;
		collider.Radius = aRadius;

		OverlapAgainstFunctionality(collider, aCallback);
	}
	template<typename TFunctionalityToReactAgainst>
	inline void OverlapLineAgainstFunctionality(const SVector2f aStart, const SVector2f aEnd, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		OverlapLineAgainstFunctionality<TFunctionalityToReactAgainst>(aStart, aEnd, 0.f, aCallback);
	}
	template<typename TFunctionalityToReactAgainst>
	inline void OverlapLineAgainstFunctionality(const SVector2f aStart, const SVector2f aEnd, const f32 aWidth, std::function<void(TFunctionalityToReactAgainst&, Colliders::SCollisionInfo)> aCallback)
	{
		Colliders::SCollisionLine collider;

		collider.Position = aStart;
		collider.EndOffsetFromPosition = aEnd - aStart;
		collider.Width = aWidth;

		OverlapAgainstFunctionality(collider, aCallback);
	}

	[[nodiscard]] static bool IsPointWithinSphere(const Colliders::SSphere& aSphere, const SVector2f aPoint) noexcept;
	[[nodiscard]] static SVector2f GetClosestPointOnLine(const Colliders::SCollisionLine& aLine, const SVector2f aPoint) noexcept;

private:

	void OverlapAgainstFunctionalityInternal(const Colliders::Shape& aCollider, const Colliders::SReaction& aReaction);

	[[nodiscard]] static bool TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept;
	[[nodiscard]] static bool TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept;
	[[nodiscard]] static bool TestAABBCollision(const Colliders::SAxisAlignedBoundingBox& aAABB, const Colliders::Shape& aCollider) noexcept;
	[[nodiscard]] static bool TestLineCollision(const Colliders::SCollisionLine& aLine, const Colliders::Shape& aCollider) noexcept;

	[[nodiscard]] static bool TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept;
	[[nodiscard]] static bool TestSphereVsAABBCollision(const Colliders::SSphere& aSphere, const Colliders::SAxisAlignedBoundingBox& aAABB) noexcept;
	[[nodiscard]] static bool TestSphereVsLineCollision(const Colliders::SSphere& aSphere, const Colliders::SCollisionLine& aLine) noexcept;
	[[nodiscard]] static bool TestLineVsLineCollision(const Colliders::SCollisionLine& aLineA, const Colliders::SCollisionLine& aLineB) noexcept;

	std::unordered_map<size_t, plf::colony<Colliders::Shape>> myCollidersByType;

	//test these every frame
	plf::colony<Colliders::Shape*> myCollidersWithReactions;
};
