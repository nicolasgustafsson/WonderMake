#pragma once

#include "Job/JobFuture.h"
#include "Job/JobPromise.h"

#include "System/System.h"
#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include "Utilities/Alias.h"
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
	template<typename... TArgs>
	class JobSystemFuture
		: private JobFuture<TArgs...>
	{
	public:
		inline JobSystemFuture(JobPromise<TArgs...>& aPromise, ScheduleSystem& aScheduleSystem, JobSystem& aJobSystem)
			: JobFuture<TArgs...>(aPromise, aScheduleSystem)
			, myJobSystem(aJobSystem)
		{}

		using JobFuture<TArgs...>::Then;

		template<typename TJob, typename... TArgs> requires std::is_base_of_v<JobBase, TJob>
		inline auto Then(TArgs&&... aArgs)
		{
			return myJobSystem.Run(std::forward<TArgs>(aArgs)...);

			auto&& jobData = myJobSystem.CreateJob<TJob>();

			JobFuture<TArgs...>::myPromise.SetCallbackSuccess([&jobSystem = myJobSystem, &jobData](TArgs&&... aArgs)
			{
				jobSystem.RunJob(jobData, std::forward<TArgs>(aArgs));
			});

			return typename TJob::template Future<JobSystemFuture>(jobData.Promise, JobFuture<TArgs...>::myScheduleSystem, myJobSystem);
		}

	private:
		JobSystem& myJobSystem;

	};

	JobSystem(SystemContainer& aSystemContainer) noexcept;

	template<typename TJob, typename... TArgs> requires std::is_base_of_v<JobBase, TJob>
	inline auto Run(TArgs&&... aArgs)
	{
		auto&& jobData = CreateJob<TJob>();

		RunJob(jobData);

		return typename TJob::template Future<JobSystemFuture>(jobData.Promise, Get<ScheduleSystem>(), *this);
	}

private:
	template<typename TJob>
	struct JobData
	{
		typename TJob::Promise Promise;
		std::shared_ptr<TJob> Job;
	};

	struct SDummyJob
		: public Job<>
	{};

	using JobDataAlias = Alias<JobData<SDummyJob>>;

	template<typename TDependencyTuple>
	struct TupleWrapper {};

	template<typename... TDependencies>
	inline std::tuple<TDependencies...> GetDependenciesHelper(TupleWrapper<std::tuple<TDependencies...>>)
	{
		return std::tie(mySystemContainer.GetSystem<std::decay_t<TDependencies>>()...);
	}

	template<typename TJob>
	inline auto& CreateJob()
	{
		const auto EmplaceJob = [this]() -> auto&
		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			return *myJobs.emplace();
		};

		auto&& jobAlias = EmplaceJob();

		auto&& jobData = jobAlias.Emplace<JobData<TJob>>();

		jobData.Promise.SetCallbackCompleted([this, &jobAlias]()
			{
				Get<ScheduleSystem>().Schedule<>([this, &jobAlias]()
					{
						std::lock_guard<decltype(myMutex)> lock(myMutex);

						myJobs.erase(myJobs.get_iterator_from_pointer(&jobAlias));
					});
			});

		return jobData;
	}
	template<typename TJob, typename... TArgs>
	inline void RunJob(JobData<TJob>& aJobData, TArgs&&... aArgs)
	{
		// TODO(Kevin): Take cargo policies into account.
		Get<ScheduleSystem>().Schedule<typename TJob::PolicySet>([this, &aJobData, ...args = aArgs]()
		{
			TJob::InjectDependencies(aJobData.Promise, GetDependenciesHelper(TupleWrapper<typename TJob::Dependencies>()));

			aJobData.Job = std::make_shared<TJob>(std::forward<TArgs>(args)...);
		});
	}

	SystemContainer& mySystemContainer;

	std::mutex myMutex;
	plf::colony<JobDataAlias> myJobs;

};
