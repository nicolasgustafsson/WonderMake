#pragma once

#include "wondermake-base/JobBase.h"
#include "wondermake-base/JobRegistry.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"
#include "wondermake-base/SystemContainer.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/UniqueFunction.h"

#include <memory>

class JobSystem
	: public System<>
	, public std::enable_shared_from_this<JobSystem>
{
public:
	JobSystem(JobRegistry aJobRegistry, SystemContainer& aSystemContainer, AnyExecutor aExecutor) noexcept;

	template<CJob TJob, typename... TArgs>
	inline [[nodiscard]] Future<typename TJob::Output> StartJob(TArgs&&... aArgs)
	{
		auto [promise, future] = MakeAsync<typename TJob::Output>();

		auto result = myJobRegistry.GetJobPolicies<TJob>();

		if (!result)
		{
			switch (result.Err())
			{
			case JobRegistry::EGetPoliciesError::JobNotRegistered:	WM_LOG_ERROR("Failed to get job policies: ", typeid(TJob).name(), ", job not registered.");
			}

			return future;
		}

		 // TODO: Take policies from result into consideration.
		myExecutor.Execute(Bind(&JobSystem::CreateAndStartJob<TJob, std::decay_t<TArgs>...>, weak_from_this(), std::move(promise), future, std::forward<TArgs>(aArgs)...));

		return future;
	}

private:
	template<CJob TJob, typename... TArgs>
	void CreateAndStartJob(Promise<typename TJob::Output> aPromise, Future<typename TJob::Output> aFuture, TArgs... aArgs)
	{
		auto result = myJobRegistry.Create<TJob>(mySystemContainer);

		if (!result)
		{
			auto& err = result.Err();

			switch (err.Error)
			{
			case JobRegistry::ECreateError::JobNotRegistered:	WM_LOG_ERROR("Failed to create job: ", typeid(TJob).name(), ", job not registered.");					break;
			case JobRegistry::ECreateError::MissingDependency:	WM_LOG_ERROR("Failed to create job: ", typeid(TJob).name(), ", missing dependency: ", err.Reason, ".");	break;
			}

			return;
		}

		SharedReference<TJob> job = std::move(result).Unwrap();

		job->Run(std::move(aPromise), std::move(aArgs)...);

		aFuture.ThenRun(myExecutor, [job = std::move(job)](auto&&) {});
	}

	JobRegistry myJobRegistry;
	SystemContainer& mySystemContainer;
	AnyExecutor myExecutor;

};
