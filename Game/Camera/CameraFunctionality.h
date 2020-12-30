#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Camera/CameraManager.h"

class Camera;

struct SCameraComponent : public SComponent
{
	TransformFunctionality2D* Target;
};

class CameraFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CameraManager, PWrite>,
			PAdd<TransformFunctionality2D, PWrite>,
			PAdd<SCameraComponent, PWrite>>>
{
public:
	void Tick();

	void SetTarget(TransformFunctionality2D* aTarget);
private:
};
