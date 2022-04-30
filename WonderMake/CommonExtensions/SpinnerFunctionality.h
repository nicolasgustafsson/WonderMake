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
: public Functionality<Wm::TimeKeeper, SSpinnerComponent, TransformFunctionality2D>
{
public:
	void Tick() noexcept;
};
