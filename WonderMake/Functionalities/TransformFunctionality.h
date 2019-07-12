#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

class TransformFunctionality
	: public Functionality<TransformFunctionality, STransformComponent>
{
public:
	TransformFunctionality(Object& aOwner);

	void SetPosition(const SVector2f aPosition);
	[[nodiscard]] SVector2f GetPosition() const;

	void SetRotation(const f32 aRotation);
	[[nodiscard]] f32 GetRotation() const;

	[[nodiscard]] SMatrix33f GetMatrix() const;

private:
	
};
