#pragma once
#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Constants.h"

class TimeKeeper;

struct SSpinnerComponent
	: public SComponent
{
	SDegreeF32 myRotationSpeed = -90;
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
