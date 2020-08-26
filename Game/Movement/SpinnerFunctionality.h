#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Constants.h"

struct SSpinnerComponent
	: public SComponent
{
	float myRotationSpeed = Constants::HalfPi;
};

class SpinnerFunctionality
	: public Functionality<
		SpinnerFunctionality,
		Policy::Set<
			Policy::Add<SSpinnerComponent, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>>>
{
public:
	SpinnerFunctionality(Object& aOwner) noexcept;

	void Tick() noexcept;

	SystemPtr<TimeKeeper> myTimeKeeper;
};

REGISTER_COMPONENT(SSpinnerComponent);
REGISTER_FUNCTIONALITY(SpinnerFunctionality);
