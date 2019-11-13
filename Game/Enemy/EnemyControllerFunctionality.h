#pragma once

#include "Components/Component.h"

#include "Enemy/TargetFunctionality.h"

#include "Functionalities/TransformFunctionality.h"

#include "Movement/DefaultMovementFunctionality.h"
#include "Movement/MovementInputFunctionality.h"

#include "Serialization/Serialization.h"

struct SEnemyControllerComponent
	: public SComponent
{
	f32 FollowRangeMin = 100.f;
};

namespace meta
{
	template <>
	inline auto registerMembers<SEnemyControllerComponent>()
	{
		return members(
			member("Follow Range Min", &SEnemyControllerComponent::FollowRangeMin)
		);
	}
}

class EnemyControllerFunctionality
	: public Functionality<
		EnemyControllerFunctionality,
		SEnemyControllerComponent,
		TransformFunctionality,
		TargetFunctionality,
		MovementInputFunctionality,
		DefaultMovementFunctionality>
{
public:
	EnemyControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

private:
};

