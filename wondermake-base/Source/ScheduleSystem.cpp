#include "wondermake-base/ScheduleSystem.h"

ScheduleSystem::ScheduleSystem(ScheduleProc aScheduleProc, ScheduleRepeatingProc aScheduleRepeatingProc) noexcept
	: myScheduleProc(std::move(aScheduleProc))
	, myScheduleRepeatingProc(std::move(aScheduleRepeatingProc))
{}
