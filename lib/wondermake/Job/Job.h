#pragma once

#include "Job/JobBase.h"
#include "Job/JobSystem.h"

#include "wondermake-base/SystemGlobal.h"

template<typename TJob, typename... TArgs>
auto RunJob(TArgs... aArgs)
{
	return Global::GetSystemContainer().Get<JobSystem>().Run<TJob>(std::forward<TArgs>(aArgs)...);
}
