#include "pch.h"

#include "ScheduleSystem.h"

ScheduleSystem::ScheduleSystem(ScheduleProc aScheduleProc, ScheduleProc aScheduleRepeatingProc) noexcept
	: myScheduleProc(std::move(aScheduleProc))
	, myScheduleRepeatingProc(std::move(aScheduleRepeatingProc))
{}

void ScheduleSystem::Schedule(Closure aTask)
{
	myScheduleProc(std::move(aTask));
}

void ScheduleSystem::ScheduleRepeating(Closure aTask)
{
	myScheduleRepeatingProc(std::move(aTask));
}
