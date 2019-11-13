#include "pch.h"
#include "DefaultMovementFunctionality.h"


DefaultMovementFunctionality::DefaultMovementFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void DefaultMovementFunctionality::Tick() noexcept
{
	STransformComponent& transform = Get<STransformComponent>();
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();
	const SVector2f movementInput = Get<SMovementInputComponent>().myMovementInput;
	const float deltaTime = myTimeKeeper->GetDeltaSeconds();

	movementComponent.myCurrentVelocity += movementInput * movementComponent.myAccelerationSpeed * deltaTime;

	if (movementComponent.myCurrentVelocity.LengthSquared() > movementComponent.myMaxMovementSpeed * movementComponent.myMaxMovementSpeed)
	{
		movementComponent.myCurrentVelocity.Normalize();
		movementComponent.myCurrentVelocity *= movementComponent.myMaxMovementSpeed;
	}

	SVector2f friction = -movementComponent.myCurrentVelocity * movementComponent.myFriction;

	movementComponent.myCurrentVelocity += friction * deltaTime;

	transform.Position += movementComponent.myCurrentVelocity * deltaTime;
}
