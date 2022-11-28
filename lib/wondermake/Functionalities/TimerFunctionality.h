#pragma once

#include "wondermake-base/Functionality.h"

#include "wondermake-utility/Typedefs.h"

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
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<STimerComponent, PWrite>>>
{
public:
	void AddTimer(const f32 aDuration, Closure aClosure);
	
	void Tick();
};
