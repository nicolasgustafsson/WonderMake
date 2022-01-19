#include "WinProgram.h"

#include "WonderMakeEngine/SystemGlobal.h"
#include "WonderMakeEngine/ScheduleSystem.h"
#include "WonderMakeEngine/WinEventSystem.h"

REGISTER_SYSTEM(WinProgram);

WinProgram::WinProgram()
{
	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });
}

void WinProgram::Tick()
{
	Get<WinEventSystem>().WaitForEvent(0);
}
