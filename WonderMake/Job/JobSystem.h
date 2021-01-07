#pragma once

#include "Job/JobFuture.h"
#include "Job/JobPromise.h"

#include "System/System.h"
#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include "Utilities/plf_colony.h"

#include <any>
#include <optional>
#include <memory>
#include <mutex>
#include <tuple>

class JobBase;

class JobSystem
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>>>
{
public:
	JobSystem(SystemContainer& aSystemContainer) noexcept;

	template<typename TJob, typename... TArgs> requires std::is_base_of_v<JobBase, TJob>
	inline typename TJob::Future Run(TArgs&&... aArgs)
	{
		std::any* jobAnyPtr = nullptr;

		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			jobAnyPtr = &*myJobs.emplace();
		}

		auto&& jobData = jobAnyPtr->emplace<JobData<TJob>>();

		jobData.Promise.SetCallbackCompleted([this, jobAnyPtr]()
			{
				Get<ScheduleSystem>().Schedule<>([this, jobAnyPtr]()
					{
						std::lock_guard<decltype(myMutex)> lock(myMutex);

						myJobs.erase(myJobs.get_iterator_from_pointer(jobAnyPtr));
					});
			});

		Get<ScheduleSystem>().Schedule<typename TJob::PolicySet>([this, &jobData, ...args = std::forward<TArgs>(aArgs)]()
		{
			TJob::InjectDependencies(jobData.Promise, GetDependenciesHelper(TupleWrapper<typename TJob::Dependencies>()));

			// TODO(Kevin): Forwarding?
			jobData.Job = std::make_shared<TJob>(args...);
		});

		return jobData.Promise;
	}

private:
	template<typename TJob>
	struct JobData
	{
		typename TJob::Promise Promise;
		std::shared_ptr<TJob> Job;
	};

	template<typename TDependencyTuple>
	struct TupleWrapper {};

	template<typename... TDependencies>
	inline std::tuple<TDependencies...> GetDependenciesHelper(TupleWrapper<std::tuple<TDependencies...>>)
	{
		return std::tie(mySystemContainer.GetSystem<std::decay_t<TDependencies>>()...);
	}

	SystemContainer& mySystemContainer;

	std::mutex myMutex;
	plf::colony<std::any> myJobs;

};
