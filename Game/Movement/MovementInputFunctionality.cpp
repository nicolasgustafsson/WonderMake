#include "pch.h"
#include "MovementInputFunctionality.h"

MovementInputFunctionality::MovementInputFunctionality(Object& aOwner)
	: Super(aOwner) {}

void MovementInputFunctionality::SetMovementInput(const SVector2f aDirection)
{
	SMovementInputComponent& input = Get<SMovementInputComponent>();

	input.myMovementInput = aDirection;
	input.myMovementInput.Normalize();
}