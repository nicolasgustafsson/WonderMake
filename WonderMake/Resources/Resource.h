#pragma once

#include "Threads/RoutineIds.h"

#define REGISTER_RESOURCE(aResource) REGISTER_RESOURCE_SYSTEM(aResource)

class Resource
{
public:
	Resource() noexcept = default;
	inline virtual ~Resource() noexcept
	{
		myRoutineChecker.RequireRoutine();
	};
	inline ERoutineId GetOwnerRoutineId() const noexcept
	{
		return myRoutineChecker.GetRoutineId();
	}
private:
	RoutineChecker myRoutineChecker;
};
