#include "pch.h"
#include "TimerFunctionality.h"
#include "Utilities/TimeKeeper.h"

REGISTER_COMPONENT(STimerComponent);
REGISTER_FUNCTIONALITY(TimerFunctionality);

f32 STimerHandle::GetTimeLeft() const
{
	return TimerFunctionality.GetTimeLeft(*this);
}

void STimerHandle::Invalidate() const
{
	TimerFunctionality.Invalidate(*this);
}

void STimerHandle::SetTimeLeft(const f32 aTimeLeft) const
{
	TimerFunctionality.SetTimeLeft(*this, aTimeLeft);
}

bool STimerHandle::IsValid() const
{
	return TimerFunctionality.IsValidTimer(*this);
}

STimerHandle TimerFunctionality::AddTimer(const f32 aDuration, Closure aClosure)
{
	return  { *(Get<STimerComponent>().Timers.insert({ aDuration, std::move(aClosure), myIdCounter.NextId()})),  *this };
}

f32 TimerFunctionality::GetTimeLeft(const STimerHandle aTimerHandle) const
{
	if (!IsValidTimer(aTimerHandle))
		return 0.f;

	return GetTimerFromHandle(aTimerHandle)->TimeLeft;
}

void TimerFunctionality::Invalidate(const STimerHandle aTimerHandle)
{
	if (!IsValidTimer(aTimerHandle))
		return;

	Get<STimerComponent>().Timers.erase(Get<STimerComponent>().Timers.get_iterator_from_pointer(GetTimerFromHandle(aTimerHandle)));
}

void TimerFunctionality::SetTimeLeft(const STimerHandle aTimerHandle, const f32 aTime)
{
	if (!IsValidTimer(aTimerHandle))
		WmLog(TagWarning, "Tried to set a non-existent timer!");

	GetTimerFromHandle(aTimerHandle)->TimeLeft = aTime;
}

bool TimerFunctionality::IsValidTimer(const STimerHandle aTimerHandle) const
{
	if (Get<STimerComponent>().Timers.empty())
		return false;

	return GetTimerFromHandle(aTimerHandle) != nullptr;
}

STimer const* TimerFunctionality::GetTimerFromHandle(const STimerHandle aHandle) const
{
	for(auto&& timer : Get<STimerComponent>().Timers)
	{
		if (timer.Id == aHandle.TimerId)
		{
			return &timer;
		}
	}

	return nullptr;
}

STimer* TimerFunctionality::GetTimerFromHandle(const STimerHandle aHandle)
{
	for (auto&& timer : Get<STimerComponent>().Timers)
	{
		if (timer.Id == aHandle.TimerId)
		{
			return &timer;
		}
	}

	return nullptr;
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
