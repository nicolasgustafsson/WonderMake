#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/ImpulseFunctionality.h"

#include "Serialization/Serialization.h"

struct SCoolImpulse;

struct SMovementInputComponent 
	: public SComponent
{
	SVector2f myMovementInput;
};

namespace meta
{
	template <>
	inline auto registerMembers<SMovementInputComponent>()
	{
		return members(
			member("MovementInput", &SMovementInputComponent::myMovementInput)
		);
	}
}

class MovementInputFunctionality
	: public Functionality<MovementInputFunctionality, SMovementInputComponent, ImpulseFunctionality>
{
public:
	MovementInputFunctionality(Object& aOwner);

	void SetMovementInput(const SVector2f aDirection);
};

