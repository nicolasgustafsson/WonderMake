#pragma once

#include "Threads/RoutineIds.h"

class Resource
{
public:
	Resource() noexcept = default;
	inline virtual ~Resource() noexcept
	{
		//myRoutineChecker.RequireRoutine();
	};
	inline ERoutineId GetOwnerRoutineId() const noexcept
	{
		return myRoutineChecker.GetRoutineId();
	}
private:
	RoutineChecker myRoutineChecker;
};
