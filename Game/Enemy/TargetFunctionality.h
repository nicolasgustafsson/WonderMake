#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

class TargetFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<TransformFunctionality, PWrite>>>
{
public:
	template<typename TPredicate>
	[[nodiscard]] inline TransformFunctionality* FindTarget(const TPredicate& aPredicate) noexcept;
};

template<typename TPredicate>
[[nodiscard]] inline TransformFunctionality* TargetFunctionality::FindTarget(const TPredicate& aPredicate) noexcept
{
	TransformFunctionality* targetTransform = nullptr;

	Get<CollisionSystem>().OverlapSphereAgainstFunctionality<CharacterFunctionality>(Get<TransformFunctionality>().GetPosition(), 300.f, [&](CharacterFunctionality& aFunctionality, Colliders::SCollisionInfo)
		{
			if (aPredicate(aFunctionality))
				targetTransform = &(aFunctionality.Get<TransformFunctionality>());
		});

	return targetTransform;
}
