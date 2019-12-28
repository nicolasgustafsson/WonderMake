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

	virtual void Tick() noexcept override;

	template<typename TIdentifyingFunctionality>
	Colliders::Shape& CreateSphereCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const f32 aRadius);

	template<typename TIdentifyingFunctionality>
	Colliders::Shape& CreateLineCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const SVector2f aSecondPosition);

	template<typename TFunctionalityToReactTo>
	void AddReaction(Colliders::Shape& aShape, std::function<void(TFunctionalityToReactTo&)> aCallback);

	bool DestroyCollider(Colliders::Shape& aCollider);

	template<typename TFunctionalityToReactTo>
	void OverlapAgainstFunctionality(const Colliders::Shape& aCollider, std::function<void(TFunctionalityToReactTo&)> aCallback);

	template<typename TFunctionalityToReactTo>
	void OverlapLineAgainstFunctionality(const SVector2f aStart, const SVector2f aEnd, std::function<void(TFunctionalityToReactTo&)> aCallback);

private:

	void OverlapAgainstFunctionalityInternal(const Colliders::Shape& aCollider, const Colliders::SReaction& aReaction);

	[[nodiscard]] static bool TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept;
	[[nodiscard]] static bool TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept;
	[[nodiscard]] static bool TestLineCollision(const Colliders::SLine& aLine, const Colliders::Shape& aCollider) noexcept;

	[[nodiscard]] static bool TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept;
	[[nodiscard]] static bool TestSphereVsLineCollision(const Colliders::SSphere& aSphereA, const Colliders::SLine& aLineB) noexcept;
	[[nodiscard]] static bool TestLineVsLineCollision(const Colliders::SLine& aLineA, const Colliders::SLine& aLineB) noexcept;

	[[nodiscard]] static bool IsPointWithinSphere(const Colliders::SSphere& aSphere, const SVector2f aPoint) noexcept;
	[[nodiscard]] static SVector2f GetClosestPointOnLine(const Colliders::SLine& aLine, const SVector2f aPoint) noexcept;

	std::unordered_map<size_t, plf::colony<Colliders::Shape>> myCollidersByType;

	//test these every frame
	plf::colony<Colliders::Shape*> myCollidersWithCallbacks;
};

template<typename TFunctionalityToReactTo>
void CollisionSystem::OverlapLineAgainstFunctionality(const SVector2f aStart, const SVector2f aEnd, std::function<void(TFunctionalityToReactTo&)> aCallback)
{
	Colliders::SLine collider;
	collider.Position = aStart;
	collider.EndOffsetFromPosition = aEnd - aStart;

	OverlapAgainstFunctionality(collider, aCallback);
}

template<typename TFunctionalityToReactTo>
void CollisionSystem::OverlapAgainstFunctionality(const Colliders::Shape& aCollider, std::function<void(TFunctionalityToReactTo&)> aCallback)
{
	Colliders::SReaction reaction(aCallback);

	OverlapAgainstFunctionalityInternal(aCollider, reaction);
}

template<typename TFunctionalityToReactTo>
void CollisionSystem::AddReaction(Colliders::Shape& aShape, std::function<void(TFunctionalityToReactTo&)> aCallback)
{
	myCollidersWithCallbacks.insert(&aShape);

	std::visit([&](auto&& aShape)
	{
		aShape.Reactions.push_back(Colliders::SReaction(aCallback));
	}, aShape);
}

template<typename TIdentifyingFunctionality>
Colliders::Shape& CollisionSystem::CreateSphereCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const f32 aRadius)
{
	Colliders::SSphere collider;

	collider.Functionality = &aFunctionality;
	collider.Position = aPosition;
	collider.Radius = aRadius;
	collider.Identifier = typeid(TIdentifyingFunctionality).hash_code();

	return (*myCollidersByType[collider.Identifier].emplace(collider));
}

template<typename TIdentifyingFunctionality>
Colliders::Shape& CollisionSystem::CreateLineCollider(TIdentifyingFunctionality& aFunctionality, const SVector2f aPosition, const SVector2f aSecondPosition)
{
	Colliders::SLine collider;

	collider.Functionality = &aFunctionality;
	collider.Position = aPosition;
	collider.EndOffsetFromPosition = aSecondPosition - aPosition;
	collider.Identifier = typeid(TIdentifyingFunctionality).hash_code();

	return (*myCollidersByType[collider.Identifier].emplace(collider));
}
