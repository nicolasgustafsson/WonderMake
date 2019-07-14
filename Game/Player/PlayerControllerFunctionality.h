#pragma once
#include "Movement/MovementInputFunctionality.h"
#include "Input/InputSystem.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"

struct SCoolImpulse
	: public SObjectImpulse
{

};

class PlayerControllerFunctionality
	: public Functionality<PlayerControllerFunctionality, TransformFunctionality, MovementInputFunctionality, DefaultMovementFunctionality, OwnerFunctionality>
	, public Debugged
{
public:
	PlayerControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:
	SystemPtr<InputSystem> myInputSystem;
};

