#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Constants.h"

class TimeKeeper;

struct SSpinnerComponent
	: public SComponent
{
	float myRotationSpeed = Constants::HalfPi;
};

class SpinnerFunctionality
	: public Functionality<
		SpinnerFunctionality,
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<SSpinnerComponent, PWrite>,
			PAdd<TransformFunctionality, PWrite>>>
{
public:
	SpinnerFunctionality(Object& aOwner, Dependencies&& aDependencies) noexcept;

	void Tick() noexcept;
};
