#pragma once
#include "Functionalities/Functionality.h"
#include "Typedefs.h"

class TimeKeeper;
class TimerFunctionality;

struct STimer
{
	f32 TimeLeft;
	Closure Callback;
	Id TimerId;
};

struct STimerComponent
	: public SComponent
{
	plf::colony<STimer> Timers;
};

struct STimerHandle
{
	STimerHandle(STimer& aTimer, TimerFunctionality& aFunctionality)
		:TimerId(aTimer.TimerId), TheTimerFunctionality(aFunctionality) {}

	STimerHandle(const STimerHandle& aTimerHandle)
		:TimerId(aTimerHandle.TimerId), TheTimerFunctionality(aTimerHandle.TheTimerFunctionality) {}

	STimerHandle(STimerHandle&& aTimerHandle) noexcept
		:TimerId(aTimerHandle.TimerId), TheTimerFunctionality(aTimerHandle.TheTimerFunctionality) {}

	[[nodiscard]] f32 GetTimeLeft() const;
	void Invalidate() const;
	void SetTimeLeft(const f32 aTime) const;
	[[nodiscard]] bool IsValid() const;

	friend class TimerFunctionality;
private:
	Id TimerId;
	TimerFunctionality& TheTimerFunctionality;
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

	void Tick() override;

	IdCounter myIdCounter;
};
