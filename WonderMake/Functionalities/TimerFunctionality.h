#pragma once
#include "Functionalities/Functionality.h"
#include "Typedefs.h"

class TimeKeeper;

struct STimerComponent
	: public SComponent
{
	struct STimer
	{
		f32 TimeLeft;
		Closure Callback;
	};

	plf::colony<STimer> Timers;
};

class TimerFunctionality
	: public Functionality<
		TimerFunctionality,
		Policy::Set<
			Policy::Add<TimeKeeper, Policy::EPermission::Read>,
			Policy::Add<STimerComponent, Policy::EPermission::Write>>>
{
public:
	TimerFunctionality(Object& aOwner, Dependencies&& aDependencies) noexcept;

	void AddTimer(const f32 aDuration, Closure aClosure);
	
	void Tick();
};
