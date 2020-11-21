#pragma once

#include "Threads/RoutineIds.h"

#define REGISTER_RESOURCE(aResource) REGISTER_RESOURCE_SYSTEM(aResource) REGISTER_ASSET_DATABASE(aResource)

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

	inline virtual bool ShouldHotReload() const { return true; }
private:
	RoutineChecker myRoutineChecker;
};
