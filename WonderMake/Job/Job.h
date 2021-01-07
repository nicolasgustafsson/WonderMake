#pragma once

#include "Job/JobBase.h"
#include "Job/JobSystem.h"

#include "System/SystemContainer.h"

template<typename TJob, typename... TArgs>
void RunJob(TArgs&&... aArgs)
{
	SystemContainer::Get().GetSystem<JobSystem>().Run<TJob>(std::forward<TArgs>(aArgs)...);
}
