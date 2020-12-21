#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

class TargetFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<TransformFunctionality2D, PWrite>>>
{
public:
	template<typename TPredicate>
	[[nodiscard]] inline TransformFunctionality2D* FindTarget(const TPredicate& aPredicate) noexcept;
};

template<typename TPredicate>
[[nodiscard]] inline TransformFunctionality2D* TargetFunctionality::FindTarget(const TPredicate& aPredicate) noexcept
{
	TransformFunctionality2D* targetTransform = nullptr;

	Get<CollisionSystem>().OverlapSphereAgainstFunctionality<CharacterFunctionality>(Get<TransformFunctionality2D>().GetPosition(), 300.f, [&](CharacterFunctionality& aFunctionality, Colliders::SCollisionInfo)
		{
			if (aPredicate(aFunctionality))
				targetTransform = &(aFunctionality.Get<TransformFunctionality2D>());
		});

	return targetTransform;
}
