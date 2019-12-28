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
		SSpinnerComponent,
		TransformFunctionality>
{
public:
	SpinnerFunctionality(Object& aOwner) noexcept;

	void Tick() noexcept;

	SystemPtr<TimeKeeper> myTimeKeeper;
};

