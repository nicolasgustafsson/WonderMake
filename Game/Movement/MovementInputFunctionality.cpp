#include "pch.h"
#include "MovementInputFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"

MovementInputFunctionality::MovementInputFunctionality(Object& aOwner)
	: Super(aOwner) 
{
	Get<ImpulseFunctionality>().Subscribe<SCoolImpulse>(*this, [&](const SCoolImpulse& aCoolImpulse)
		{
			WmLog("Wow I feel so much cooler now! Many thanks!");

			Get<ImpulseFunctionality>().Unsubscribe<SCoolImpulse>(*this);
		});
}


void MovementInputFunctionality::SetMovementInput(const SVector2f aDirection)
{
	SMovementInputComponent& input = Get<SMovementInputComponent>();

	input.myMovementInput = aDirection;
	input.myMovementInput.Normalize();
}
