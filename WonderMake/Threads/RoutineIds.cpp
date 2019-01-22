#include "stdafx.h"

#include "RoutineIds.h"

thread_local ERoutineId CurrentRoutine;

void _SetCurrentRoutine(const ERoutineId aRoutineId)
{
	CurrentRoutine = aRoutineId;
}

ERoutineId _GetCurrentRoutine()
{
	return CurrentRoutine;
}
