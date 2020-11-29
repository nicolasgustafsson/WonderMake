#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Constants.h"

class TimeKeeper;

struct SSpinnerComponent
	: public SComponent
{
	SRadianF32 myRotationSpeed = Constants::HalfPi;
};

class SpinnerFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<SSpinnerComponent, PWrite>,
			PAdd<TransformFunctionality, PWrite>>>
{
public:
	void Tick() noexcept;
};
