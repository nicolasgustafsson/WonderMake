#include "pch.h"
#include "TimeToLiveFunctionality.h"
#include "Functionalities/TimerFunctionality.h"
#include "Levels/ObjectContainerFunctionality.h"

REGISTER_FUNCTIONALITY(TimeToLiveFunctionality);

void TimeToLiveFunctionality::SetTimeToLive(const f32 aTimeToLive)
{
	Get<TimerFunctionality>().AddTimer(aTimeToLive, [&]() 
		{
			Get<SSubobjectComponent>().SlatedForRemoval = true;
		});
}
