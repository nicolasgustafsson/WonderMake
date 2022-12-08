#pragma once

#include "wondermake-base/System.h"

class ScheduleSystem;
class WinEventSystem;

class WinProgram
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>,
			PAdd<WinEventSystem, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	WinProgram();

private:
	void Tick();

};
