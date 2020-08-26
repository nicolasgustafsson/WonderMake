#pragma once
#include "Functionalities/TransformFunctionality.h"

struct SCameraComponent : public SComponent
{
	TransformFunctionality* Target;
};

class CameraFunctionality
	: public Functionality<
		CameraFunctionality,
		Policy::Set<
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<SCameraComponent, Policy::EPermission::Write>>>
{
public:
	CameraFunctionality(Object& aObject);

	void Tick();

	void SetTarget(TransformFunctionality* aTarget);
private:
};

REGISTER_FUNCTIONALITY(CameraFunctionality);
