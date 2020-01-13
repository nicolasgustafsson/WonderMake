#pragma once
#include "Functionalities/TransformFunctionality.h"

struct SCameraComponent : public SComponent
{
	TransformFunctionality* Target;
};

class CameraFunctionality : public Functionality<CameraFunctionality, TransformFunctionality, SCameraComponent>
{
public:
	CameraFunctionality(Object& aObject);

	void Tick();

	void SetTarget(TransformFunctionality* aTarget);
private:
};

