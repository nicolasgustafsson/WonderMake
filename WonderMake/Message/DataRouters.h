#pragma once

#include "DispatchRouter.h"

#include "Threads/RoutineIds.h"

#include "Utilities/Singleton.h"

class DataRouters final
	: public Singleton<DataRouters>
{
public:
	DataRouters() = default;

	inline constexpr DispatchRouter& GetRouter(const ERoutineId aRoutineId) noexcept;

private:
	DispatchRouter myRouters[RoutineCount];
};

constexpr DispatchRouter& DataRouters::GetRouter(const ERoutineId aRoutineId) noexcept
{
	return myRouters[static_cast<u32>(aRoutineId)];
}
