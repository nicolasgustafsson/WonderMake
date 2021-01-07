#pragma once

#include "Job/JobBase.h"
#include "Job/JobSystem.h"

#include "System/SystemContainer.h"

template<typename TJob, typename... TArgs>
auto RunJob(TArgs&&... aArgs)
{
	return SystemContainer::Get().GetSystem<JobSystem>().Run<TJob>(std::forward<TArgs>(aArgs)...);
}
