#include "pch.h"

#include "TimerFunctionality.h"

#include "Utilities/TimeKeeper.h"

WM_REGISTER_COMPONENT(STimerComponent);
WM_REGISTER_FUNCTIONALITY(TimerFunctionality);

void TimerFunctionality::AddTimer(const f32 aDuration, Closure aClosure)
{
	Get<STimerComponent>().Timers.insert({ aDuration, std::move(aClosure)});
}

void TimerFunctionality::Tick() 
{
	//[Nicos]: We use two loops in this function to protect against adding timers in callbacks

	STimerComponent& timerComponent = Get<STimerComponent>();

	const f32 deltaTime = Get<TimeKeeper>().GetDeltaTime<WmChrono::fSeconds>().count();

	for (auto&& timer : timerComponent.Timers)
	{
		timer.TimeLeft -= deltaTime;

		if (timer.TimeLeft < 0.f)
			std::move(timer).Callback();
	}

	auto&& it = timerComponent.Timers.begin();

	while (it != timerComponent.Timers.end())
	{
		auto&& timer = *it;

		if (timer.TimeLeft < 0.f)
			it = timerComponent.Timers.erase(it);
		else
			it++;
	}
}
