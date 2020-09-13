#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

class TargetFunctionality
	: public Functionality<
		TargetFunctionality,
		Policy::Set<
			Policy::Add<CollisionSystem, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>>>
{
public:
	TargetFunctionality(Object& aOwner, Dependencies&& aDependencies);

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
