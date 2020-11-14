#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"

struct SCoolImpulse;

struct SMovementInputComponent 
	: public SComponent
{
	SVector2f myMovementInput;
};

class MovementInputFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<SMovementInputComponent, PWrite>>>
{
public:
	void SetMovementInput(const SVector2f aDirection);
};
