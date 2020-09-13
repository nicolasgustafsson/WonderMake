#include "pch.h"
#include "TimerFunctionality.h"
#include "Utilities/TimeKeeper.h"

REGISTER_COMPONENT(STimerComponent);
REGISTER_FUNCTIONALITY(TimerFunctionality);

TimerFunctionality::TimerFunctionality(Object& aOwner, Dependencies&& aDependencies) noexcept
	: Super(aOwner, std::move(aDependencies))
{

}

void TimerFunctionality::AddTimer(const f32 aDuration, Closure aClosure)
{
	Get<STimerComponent>().Timers.insert({ aDuration, std::move(aClosure)});
}

void TimerFunctionality::Tick() 
{
	//[Nicos]: We use two loops in this function to protect against adding timers in callbacks

	STimerComponent& timerComponent = Get<STimerComponent>();

	const f32 deltaTime = Get<TimeKeeper>().GetDeltaSeconds();

	for (auto&& timer : timerComponent.Timers)
	{
		timer.TimeLeft -= deltaTime;

		if (timer.TimeLeft < 0.f)
			timer.Callback();
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
