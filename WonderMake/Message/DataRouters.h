#pragma once

#include "DispatchRouter.h"

#include "Threads/ThreadIds.h"

#include "Utilities/Singleton.h"

class DataRouters final
	: public Singleton<DataRouters>
{
public:
	DataRouters() = default;

	inline DispatchRouter& GetRouter(const EThreadId aProcess);

private:
	DispatchRouter myRouters[ThreadCount];
};

DispatchRouter& DataRouters::GetRouter(const EThreadId aProcess)
{
	return myRouters[static_cast<u32>(aProcess)];
}
