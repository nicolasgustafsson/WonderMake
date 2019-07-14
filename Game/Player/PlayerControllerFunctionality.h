#pragma once
#include "Movement/MovementInputFunctionality.h"
#include "Input/InputSystem.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

class PlayerControllerFunctionality
	: public Functionality<PlayerControllerFunctionality, TransformFunctionality, MovementInputFunctionality, DefaultMovementFunctionality>
	, public Debugged
{
public:
	PlayerControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:
	SystemPtr<InputSystem> myInputSystem;
};

