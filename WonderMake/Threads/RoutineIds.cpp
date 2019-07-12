#include "stdafx.h"

#include "RoutineIds.h"

thread_local ERoutineId CurrentRoutine;

void _SetCurrentRoutine(const ERoutineId aRoutineId) noexcept
{
	CurrentRoutine = aRoutineId;
}

ERoutineId _GetCurrentRoutine() noexcept
{
	return CurrentRoutine;
}
