#pragma once
#include "Functionalities/Functionality.h"
#include "Typedefs.h"

class TimeKeeper;

struct STimer
{
	f32 TimeLeft;
	Closure Callback;
};

struct STimerComponent
	: public SComponent
{
	plf::colony<STimer> Timers;
};

struct STimerHandle
{
	STimerHandle(STimer& aTimer)
		:Timer(aTimer) {}

	STimerHandle(const STimerHandle& aTimerHandle)
		:Timer(aTimerHandle.Timer) {}

	STimerHandle(STimerHandle&& aTimerHandle) noexcept
		:Timer(aTimerHandle.Timer) {}

	friend class TimerFunctionality;
private:
	STimer& Timer;
};

class TimerFunctionality
	: public Functionality<TimeKeeper, STimerComponent>
{
public:
	STimerHandle AddTimer(const f32 aDuration, Closure aClosure);

	f32 GetTimeLeft(const STimerHandle aTimerHandle) const;

	void Invalidate(const STimerHandle aTimerHandle);

	void ResetTimer(const STimerHandle aTimerHandle, const f32 aTime);

	bool IsValidTimer(const STimerHandle aTimerHandle) const;
	
	void Tick();
};
