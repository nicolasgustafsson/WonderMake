#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/ImpulseFunctionality.h"

struct SCoolImpulse;

struct SMovementInputComponent 
	: public SComponent
{
	SVector2f myMovementInput;
};

class MovementInputFunctionality
	: public Functionality<
		MovementInputFunctionality,
		Policy::Set<
			Policy::Add<SMovementInputComponent, Policy::EPermission::Write>>>
{
public:
	MovementInputFunctionality(Object& aOwner, Dependencies&& aDependencies);

	void SetMovementInput(const SVector2f aDirection);
};
