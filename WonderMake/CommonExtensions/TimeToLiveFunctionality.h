#pragma once
#include "Functionalities/TimerFunctionality.h"
#include "Levels/SubobjectFunctionality.h"
#include "Functionalities/Functionality.h"

class TimeToLiveFunctionality : public Functionality<
	Policy::Set<
		PAdd<SSubobjectComponent, PWrite>, 
		PAdd<TimerFunctionality, PWrite>>>
{
public:
	void SetTimeToLive(const f32 aTimeToLive);
};
