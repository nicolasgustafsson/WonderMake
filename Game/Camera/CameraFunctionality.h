#pragma once
#include "Functionalities/TransformFunctionality.h"

class Camera;

struct SCameraComponent : public SComponent
{
	TransformFunctionality* Target;
};

class CameraFunctionality
	: public Functionality<
		CameraFunctionality,
		Policy::Set<
			PAdd<Camera, PWrite>,
			PAdd<TransformFunctionality, PWrite>,
			PAdd<SCameraComponent, PWrite>>>
{
public:
	CameraFunctionality(Object& aObject, Dependencies&& aDependencies);

	void Tick();

	void SetTarget(TransformFunctionality* aTarget);
private:
};
