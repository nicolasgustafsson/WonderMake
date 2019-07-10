#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

class TransformFunctionality
	: public Functionality<TransformFunctionality, STransformComponent>
{
public:
	TransformFunctionality(Object& aOwner);

	[[no_discard]] SVector2f GetPosition() const;

private:
	
};
