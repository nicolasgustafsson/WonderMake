#pragma once

#include "Input/InputSystem.h"
#include "Movement/MovementInputFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"

#include "Character/CharacterFunctionality.h"

#include "Weapons/MeleeWeaponUserFunctionality.h"
#include "Collision/CollisionFunctionality.h"

#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"

struct SCoolImpulse
	: public SObjectImpulse<SCoolImpulse>
{

};

class PlayerControllerFunctionality
	: public Functionality<
		PlayerControllerFunctionality, TransformFunctionality,
		MovementInputFunctionality, DefaultMovementFunctionality,
		OwnerFunctionality, MeleeWeaponUserFunctionality,
		ActionFunctionality, CollisionFunctionality,
		CharacterFunctionality>
	, public Debugged
{
public:
	PlayerControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:
	void UpdateMovement();

	SystemPtr<InputSystem> myInputSystem;
};

