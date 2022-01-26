#include "WinProgram.h"

#include "WonderMakeBase/SystemGlobal.h"
#include "WonderMakeBase/ScheduleSystem.h"
#include "WonderMakeBase/WinEventSystem.h"

REGISTER_SYSTEM(WinProgram);

WinProgram::WinProgram()
{
	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });
}

void WinProgram::Tick()
{
	Get<WinEventSystem>().WaitForEvent(0);
}
