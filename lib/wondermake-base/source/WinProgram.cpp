#include "WinProgram.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/WinEventSystem.h"

WM_REGISTER_SYSTEM(WinProgram);

WinProgram::WinProgram()
{
	Get<ScheduleSystemSingleton>()
		.ScheduleRepeating(GetExecutor(), [this]() { Tick(); })
		.Detach();
}

void WinProgram::Tick()
{
	Get<WinEventSystem>().WaitForEvent(0);
}
