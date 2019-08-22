#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

class TransformFunctionality
	: public Functionality<TransformFunctionality, STransformComponent>
{
public:
	TransformFunctionality(Object& aOwner);

	[[nodiscard]] SVector2f GetPosition() const;

	void SetPosition(const SVector2f aPosition);

	void Inspect();

private:
};
