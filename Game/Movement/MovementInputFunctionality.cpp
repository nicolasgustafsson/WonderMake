#include "pch.h"
#include "MovementInputFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"

REGISTER_COMPONENT(SMovementInputComponent);
REGISTER_FUNCTIONALITY(MovementInputFunctionality);

void MovementInputFunctionality::SetMovementInput(const SVector2f aDirection)
{
	SMovementInputComponent& input = Get<SMovementInputComponent>();

	input.myMovementInput = aDirection;
	input.myMovementInput.Normalize();
}