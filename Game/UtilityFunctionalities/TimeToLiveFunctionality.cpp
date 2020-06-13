#include "pch.h"
#include "TimeToLiveFunctionality.h"
#include "Functionalities/TimerFunctionality.h"

void TimeToLiveFunctionality::SetTimeToLive(const f32 aTimeToLive)
{
	Get<TimerFunctionality>().AddTimer(aTimeToLive, []() {WmLog("Time to die"); });
}
