#pragma once
#include "MovementInputFunctionality.h"

#include "Serialization/Serialization.h"

struct SDefaultMovementComponent
	: public SComponent
{
	SVector2f myCurrentVelocity;
	float myAccelerationSpeed = 6700.f;
	float myFriction = 7.0f;
	float myMaxMovementSpeed = 911.f;
};

namespace meta
{
	template <>
	inline auto registerMembers<SDefaultMovementComponent>()
	{
		return members(
			member("Current Velocity", &SDefaultMovementComponent::myCurrentVelocity),
			member("Acceleration Speed", &SDefaultMovementComponent::myAccelerationSpeed),
			member("Friction", &SDefaultMovementComponent::myFriction),
			member("Max Movement Speed", &SDefaultMovementComponent::myMaxMovementSpeed)
		);
	}
}

class DefaultMovementFunctionality
	: public Functionality<DefaultMovementFunctionality, SMovementInputComponent, STransformComponent, SDefaultMovementComponent>
{
public:
	DefaultMovementFunctionality(Object& aOwner);

	void Tick() noexcept;

	SystemPtr<TimeKeeper> myTimeKeeper;
};

