#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"

struct SMovementInputComponent 
	: public SComponent
{
	SVector2f myMovementInput;
};

class MovementInputFunctionality :
	public Functionality<MovementInputFunctionality, SMovementInputComponent>
{
public:
	MovementInputFunctionality(Object& aOwner);

	void SetMovementInput(const SVector2f aDirection);
};

