#pragma once
#include "Movement/MovementInputFunctionality.h"
#include "Input/InputSystem.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"
#include "Weapons/MeleeWeaponUserFunctionality.h"
#include "Collision/CollisionFunctionality.h"

struct SCoolImpulse
	: public SObjectImpulse
{

};

class PlayerControllerFunctionality
	: public Functionality<
			PlayerControllerFunctionality, TransformFunctionality, 
			MovementInputFunctionality, DefaultMovementFunctionality, 
			OwnerFunctionality, MeleeWeaponUserFunctionality,
			ActionFunctionality, CollisionFunctionality>
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

