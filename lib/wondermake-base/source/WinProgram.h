#pragma once

#include "wondermake-base/System.h"

class ScheduleSystemSingleton;
class WinEventSystem;

class WinProgram
	: public System<
		Policy::Set<
			PAdd<ScheduleSystemSingleton, PWrite>,
			PAdd<WinEventSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	WinProgram();

private:
	void Tick();

};
