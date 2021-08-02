#pragma once
#include "Functionalities/TimerFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/Functionality.h"

class TimeToLiveFunctionality : public Functionality<
	Policy::Set<
		PAdd<SLevelDenizenComponent, PWrite>, 
		PAdd<TimerFunctionality, PWrite>>>
{
public:
	void SetTimeToLive(const f32 aTimeToLive);
};
