#pragma once

#include "DispatchRouter.h"

#include "Threads/RoutineIds.h"

#include "Utilities/Singleton.h"

class DataRouters final
	: public Singleton<DataRouters>
{
public:
	DataRouters() = default;

	inline DispatchRouter& GetRouter(const ERoutineId aRoutineId);

private:
	DispatchRouter myRouters[RoutineCount];
};

DispatchRouter& DataRouters::GetRouter(const ERoutineId aRoutineId)
{
	return myRouters[static_cast<u32>(aRoutineId)];
}
