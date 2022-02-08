#include "WinProgram.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/WinEventSystem.h"

REGISTER_SYSTEM(WinProgram);

WinProgram::WinProgram()
{
	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });
}

void WinProgram::Tick()
{
	Get<WinEventSystem>().WaitForEvent(0);
}
