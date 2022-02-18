#pragma once

#ifndef WM_SUPPRESS_JOB_GLOBAL

#include "wondermake-base/JobRegistry.h"

#include "wondermake-utility/AutoRegister.h"

class JobGlobal
{
public:
	static JobRegistry& GetRegistry();
};

template<CJob TJob, CJob TJobBase>
static void _RegisterJob() requires(std::is_same_v<TJobBase, TJob> || std::is_base_of_v<TJobBase, TJob>)
{
	JobGlobal::GetRegistry().AddJob<TJob, TJobBase>([](auto&& aDependencies) -> SharedReference<TJob>
		{
			TJob::InjectDependencies(std::move(aDependencies));
			
			return MakeSharedReference<TJob>();
		});
}

#define _WM_REGISTER_JOB_CLASS(aJob, aJobBase) _RegisterJob< aJob , aJobBase >

#define _WM_REGISTER_JOB_MASKED_IMPL(aJob, aJobBase, aJobName) WM_AUTO_REGISTER(_WM_REGISTER_JOB_CLASS( aJob , aJobBase ), aJobName )

#define WM_REGISTER_JOB_MASKED(aJob, aJobBase) _WM_REGISTER_JOB_MASKED_IMPL( aJob , aJobBase , aJob)
#define WM_REGISTER_JOB(aJob) _WM_REGISTER_JOB_MASKED_IMPL( aJob , aJob , aJob)

#else

#define WM_REGISTER_JOB_MASKED(aJob, aJobBase)
#define WM_REGISTER_JOB(aJob)

#endif
