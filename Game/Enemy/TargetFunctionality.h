#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

class TargetFunctionality
	: public Functionality<
		TargetFunctionality,
		Policy::Set<
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>>>
{
public:
	TargetFunctionality(Object& aOwner);

	template<typename TPredicate>
	[[nodiscard]] inline TransformFunctionality* FindTarget(const TPredicate& aPredicate) const noexcept;
};

template<typename TPredicate>
[[nodiscard]] inline TransformFunctionality* TargetFunctionality::FindTarget(const TPredicate& aPredicate) const noexcept
{
	TransformFunctionality* targetTransform = nullptr;

	SystemPtr<CollisionSystem>()->OverlapSphereAgainstFunctionality<CharacterFunctionality>(Get<TransformFunctionality>().GetPosition(), 300.f, [&](CharacterFunctionality& aFunctionality, Colliders::SCollisionInfo)
		{
			if (aPredicate(aFunctionality))
				targetTransform = &(aFunctionality.Get<TransformFunctionality>());
		});

	return targetTransform;
}

REGISTER_FUNCTIONALITY(TargetFunctionality);
