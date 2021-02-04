#pragma once

#include "Typedefs.h"

#include "System/System.h"

#include <functional>

class ScheduleSystem
	: public System<>
{
public:
	using ScheduleProc = std::function<void(Closure)>;

	ScheduleSystem(ScheduleProc aScheduleProc, ScheduleProc aScheduleRepeatingProc) noexcept;

	void Schedule(Closure aTask);
	void ScheduleRepeating(Closure aTask);

private:
	ScheduleProc myScheduleProc;
	ScheduleProc myScheduleRepeatingProc;

};
