#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Camera/CameraManager.h"

class Camera;

struct SCameraComponent : public SComponent
{
	TransformFunctionality* Target;
};

class CameraFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CameraManager, PWrite>,
			PAdd<TransformFunctionality, PWrite>,
			PAdd<SCameraComponent, PWrite>>>
{
public:
	void Tick();

	void SetTarget(TransformFunctionality* aTarget);
private:
};
