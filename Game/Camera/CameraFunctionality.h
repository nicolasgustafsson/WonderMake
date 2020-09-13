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
			Policy::Add<Camera, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<SCameraComponent, Policy::EPermission::Write>>>
{
public:
	CameraFunctionality(Object& aObject, Dependencies&& aDependencies);

	void Tick();

	void SetTarget(TransformFunctionality* aTarget);
private:
};
