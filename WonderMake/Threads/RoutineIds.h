#pragma once

#include <assert.h>

#include "Typedefs.h"

constexpr u32 RoutineCount = 3;

enum class ERoutineId
{
	Logic = 0,
	Render,
	File
};

void _SetCurrentRoutine(const ERoutineId aRoutineId);
ERoutineId _GetCurrentRoutine();

class RoutineChecker
{
public:
	RoutineChecker() = default;
	inline RoutineChecker(const ERoutineId aRoutineId)
		: myRoutineId(aRoutineId)
	{}
	inline void AttachToCurrentRoutine()
	{
		myRoutineId = _GetCurrentRoutine();
	}
	inline bool IsCurrentRoutine() const
	{
		return myRoutineId == _GetCurrentRoutine();
	}
	inline void RequireRoutine() const
	{
		assert(IsCurrentRoutine());
	}
	inline void SetRoutineId(const ERoutineId aRoutineId)
	{
		myRoutineId = aRoutineId;
	}
	inline ERoutineId GetRoutineId() const
	{
		return myRoutineId;
	}
private:
	ERoutineId myRoutineId = _GetCurrentRoutine();
};
