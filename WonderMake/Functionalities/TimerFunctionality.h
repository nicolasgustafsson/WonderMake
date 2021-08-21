#pragma once
#include "Functionalities/Functionality.h"
#include "Typedefs.h"

class TimeKeeper;
class TimerFunctionality;

struct STimer
{
	f32 TimeLeft;
	Closure Callback;
	Id Id;
};

struct STimerComponent
	: public SComponent
{
	plf::colony<STimer> Timers;
};

struct STimerHandle
{
	STimerHandle(STimer& aTimer, TimerFunctionality& aFunctionality)
		:TimerId(aTimer.Id), TimerFunctionality(aFunctionality) {}

	STimerHandle(const STimerHandle& aTimerHandle)
		:TimerId(aTimerHandle.TimerId), TimerFunctionality(aTimerHandle.TimerFunctionality) {}

	STimerHandle(STimerHandle&& aTimerHandle) noexcept
		:TimerId(aTimerHandle.TimerId), TimerFunctionality(aTimerHandle.TimerFunctionality) {}

	f32 GetTimeLeft() const;
	void Invalidate() const;
	void SetTimeLeft(const f32 aTime) const;
	bool IsValid() const;

	friend class TimerFunctionality;
private:
	Id TimerId;
	TimerFunctionality& TimerFunctionality;
};

class TimerFunctionality
	: public Functionality<TimeKeeper, STimerComponent>
{
public:
	STimerHandle AddTimer(const f32 aDuration, Closure aClosure = [](){});

	f32 GetTimeLeft(const STimerHandle aTimerHandle) const;

	void Invalidate(const STimerHandle aTimerHandle);

	void SetTimeLeft(const STimerHandle aTimerHandle, const f32 aTime);

	bool IsValidTimer(const STimerHandle aTimerHandle) const;

	STimer const* GetTimerFromHandle(const STimerHandle aTimerHandle) const;
	STimer* GetTimerFromHandle(const STimerHandle aTimerHandle);

	void Tick();

	IdCounter myIdCounter;
};
