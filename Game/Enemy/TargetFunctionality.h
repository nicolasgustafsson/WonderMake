#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

class TargetFunctionality
	: public Functionality<TargetFunctionality, TransformFunctionality>
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
	Colliders::SSphere sphere;

	sphere.Position = Get<TransformFunctionality>().GetPosition();
	sphere.Radius = 1000.f;

	SystemPtr<CollisionSystem>()->OverlapAgainstFunctionality<CharacterFunctionality>(sphere, [&](CharacterFunctionality& aFunctionality)
		{
			if (aPredicate(aFunctionality))
				targetTransform = &(aFunctionality.Get<TransformFunctionality>());
		});

	return targetTransform;
}
