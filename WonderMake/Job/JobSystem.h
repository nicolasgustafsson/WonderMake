#pragma once

#include "Job/JobFuture.h"
#include "Job/JobOutput.h"
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
	template<typename TOutput, typename TOutputError>
	class JobSystemFuture
		: private JobFuture<TOutput, TOutputError>
	{
	public:
		using Super = JobFuture<TOutput, TOutputError>;
		using Promise = JobPromise<TOutput, TOutputError>;

		inline JobSystemFuture(Promise& aPromise, ScheduleSystem& aScheduleSystem, JobSystem& aJobSystem)
			: JobFuture<TOutput, TOutputError>(aPromise, aScheduleSystem)
			, myJobSystem(aJobSystem)
		{}

		using Super::Then;
		using Super::Error;

		template<typename TJob> requires std::is_base_of_v<JobBase, TJob>
		inline auto Then()
		{
			return JobSystemImpl<TOutput>::template CreateJob<TJob>(Super::myPromise, Super::myScheduleSystem, myJobSystem);
		}

	private:
		template<typename UOutput>
		class JobSystemImpl
		{
			static_assert(AlwaysFalse<UOutput>, "Job has invalid input.");
		};
		template<typename... TArgs>
		class JobSystemImpl<JobOutput<TArgs...>>
		{
		public:
			template<typename TJob>
			inline static auto CreateJob(Promise& aPromise, ScheduleSystem& aScheduleSystem, JobSystem& aJobSystem)
			{
				static_assert(std::is_constructible_v<TJob, TArgs...>, "Job input does not match output.");

				auto&& jobData = aJobSystem.CreateJob<TJob>();

				aPromise.SetCallbackSuccess([&aJobSystem, &jobData](TArgs&&... aArgs)
					{
						aJobSystem.RunJob(jobData, std::forward<TArgs>(aArgs)...);
					});

				return JobSystemFuture<typename TJob::Output, typename TJob::OutputError>(jobData.Promise, aScheduleSystem, aJobSystem);
			}
		};

		JobSystem& myJobSystem;

	};

	JobSystem(SystemContainer& aSystemContainer) noexcept;

	template<typename TJob, typename... TArgs> requires std::is_base_of_v<JobBase, TJob>
	inline auto Run(TArgs&&... aArgs)
	{
		auto&& jobData = CreateJob<TJob>();

		RunJob(jobData, std::forward<TArgs>(aArgs)...);

		return JobSystemFuture<typename TJob::Output, typename TJob::OutputError>(jobData.Promise, Get<ScheduleSystem>(), *this);
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

		auto&& jobData = jobAlias.template Emplace<JobData<TJob>>();

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
		Get<ScheduleSystem>().Schedule<typename TJob::PolicySet>([this, &aJobData, ...args = aArgs]() mutable
		{
			TJob::InjectDependencies(aJobData.Promise, GetDependenciesHelper(TupleWrapper<typename TJob::Dependencies>()));

			aJobData.Job = std::make_shared<TJob>(std::move(args)...);
		});
	}

	SystemContainer& mySystemContainer;

	std::mutex myMutex;
	plf::colony<JobDataAlias> myJobs;

};
