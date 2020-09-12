#pragma once
#include "Functionalities/Functionality.h"
#include "Typedefs.h"

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
			Policy::Add<STimerComponent, Policy::EPermission::Write>>>
{
public:
	TimerFunctionality(Object& aOwner) noexcept;

	void AddTimer(const f32 aDuration, Closure aClosure);
	
	void Tick();
};

REGISTER_COMPONENT(STimerComponent);
REGISTER_FUNCTIONALITY(TimerFunctionality);
