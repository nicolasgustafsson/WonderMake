#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/jobs/JobBase.h"
#include "wondermake-base/jobs/JobRegistry.h"

template<jobs_refactor::CJob TJob>
class JobMock
	: public TJob
	, public jobs_refactor::JobSub<>
	, public std::enable_shared_from_this<JobMock<TJob>>
{
public:
	inline [[nodiscard]] auto AddToRegistry(JobRegistry& aRegistry)
	{
		auto result = SharedReference<JobMock>::FromPointer(this->shared_from_this());

		SharedReference<JobMock> selfReference = result;

		auto createFunc = [self = std::move(selfReference)](auto&&) -> SharedReference<JobMock>
		{
			return self;
		};

		aRegistry.AddJob<JobMock, TJob>(std::move(createFunc));
	}
};

template<typename TMock>
inline auto CreateJobMock()
{
	TMock::InjectDependencies(std::tie());

	return MakeSharedReference<TMock>();
}
