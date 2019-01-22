#pragma once

#include "Threads/RoutineIds.h"

class Resource
{
public:
	Resource() = default;
	inline virtual ~Resource()
	{
		myRoutineChecker.RequireRoutine();
	};
	inline ERoutineId GetOwnerRoutineId() const
	{
		return myRoutineChecker.GetRoutineId();
	}
private:
	RoutineChecker myRoutineChecker;
};
