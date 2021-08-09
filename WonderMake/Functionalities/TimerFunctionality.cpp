#include "pch.h"
#include "TimerFunctionality.h"
#include "Utilities/TimeKeeper.h"

REGISTER_COMPONENT(STimerComponent);
REGISTER_FUNCTIONALITY(TimerFunctionality);

STimerHandle TimerFunctionality::AddTimer(const f32 aDuration, Closure aClosure)
{
	return  { *(Get<STimerComponent>().Timers.insert({ aDuration, std::move(aClosure)})) };
}

f32 TimerFunctionality::GetTimeLeft(const STimerHandle aTimerHandle) const
{
	if (!IsValidTimer(aTimerHandle))
		return 0.f;

	return Get<STimerComponent>().Timers.get_iterator_from_pointer(&aTimerHandle.Timer)->TimeLeft;
}

void TimerFunctionality::Invalidate(const STimerHandle aTimerHandle)
{
	if (!IsValidTimer(aTimerHandle))
		return;

	Get<STimerComponent>().Timers.erase(Get<STimerComponent>().Timers.get_iterator_from_pointer(&aTimerHandle.Timer));
}

void TimerFunctionality::ResetTimer(const STimerHandle aTimerHandle, const f32 aTime)
{
	if (!IsValidTimer(aTimerHandle))
		WmLog(TagWarning, "Tried to set a non-existent timer!");

	Get<STimerComponent>().Timers.get_iterator_from_pointer(&aTimerHandle.Timer)->TimeLeft = aTime;
}

bool TimerFunctionality::IsValidTimer(const STimerHandle aTimerHandle) const
{
	if (Get<STimerComponent>().Timers.empty())
		return false;

	return Get<STimerComponent>().Timers.get_iterator_from_pointer(&aTimerHandle.Timer) != Get<STimerComponent>().Timers.end();
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
