#include "pch.h"

#include "ScheduleSystem.h"

ScheduleSystem::ScheduleSystem(ScheduleProc aScheduleProc, ScheduleProc aScheduleRepeatingProc) noexcept
	: myScheduleProc(std::move(aScheduleProc))
	, myScheduleRepeatingProc(std::move(aScheduleRepeatingProc))
{}
