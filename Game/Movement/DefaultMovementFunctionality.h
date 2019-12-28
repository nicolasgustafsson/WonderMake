#pragma once
#include "MovementInputFunctionality.h"

struct SDefaultMovementComponent
	: public SComponent
{
	SVector2f myCurrentVelocity;
	float myAccelerationSpeed = 6700.f;
	float myFriction = 7.0f;
	float myMaxMovementSpeed = 911.f;
};

class DefaultMovementFunctionality
	: public Functionality<DefaultMovementFunctionality, SMovementInputComponent, STransformComponent, SDefaultMovementComponent>
{
public:
	DefaultMovementFunctionality(Object& aOwner);

	void AddForce(const SVector2f aForce);
	void AddImpulse(const SVector2f aImpulse);

	void Tick() noexcept;

	void Inspect();

	SystemPtr<TimeKeeper> myTimeKeeper;
};

