#pragma once

#include <assert.h>

#include "Typedefs.h"


enum class ERoutineId
{
	Logic = 0,
	Render,
	File,
	Debug,
	_Count
};

constexpr u32 RoutineCount = static_cast<u32>(ERoutineId::_Count);

void _SetCurrentRoutine(const ERoutineId aRoutineId) noexcept;
ERoutineId _GetCurrentRoutine() noexcept;

class RoutineChecker
{
public:
	RoutineChecker() = default;
	inline RoutineChecker(const ERoutineId aRoutineId) noexcept
		: myRoutineId(aRoutineId)
	{}
	inline void AttachToCurrentRoutine() noexcept
	{
		myRoutineId = _GetCurrentRoutine();
	}
	inline bool IsCurrentRoutine() const noexcept
	{
		return myRoutineId == _GetCurrentRoutine();
	}
	inline void RequireRoutine() const noexcept
	{
		assert(IsCurrentRoutine());
	}
	inline void SetRoutineId(const ERoutineId aRoutineId) noexcept
	{
		myRoutineId = aRoutineId;
	}
	inline ERoutineId GetRoutineId() const noexcept
	{
		return myRoutineId;
	}
private:
	ERoutineId myRoutineId = _GetCurrentRoutine();
};
