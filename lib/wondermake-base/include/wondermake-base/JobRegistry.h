#pragma once

#include "wondermake-base/JobBase.h"
#include "wondermake-base/SystemContainer.h"

#include "wondermake-utility/Result.h"
#include "wondermake-utility/SharedReference.h"

#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>

class JobRegistry
{
public:
	enum class EGetPoliciesError
	{
		JobNotRegistered
	};

	enum class ECreateError
	{
		JobNotRegistered,
		MissingDependency
	};

	template<
		jobs_refactor::CJob TJob,
		typename TBaseJob = TJob,
		typename TCreateFunc>
	inline void AddJob(TCreateFunc&& aCreateFunc)
		requires(std::is_copy_constructible_v<TCreateFunc>
			&& std::is_base_of_v<TBaseJob, TJob>)
	{
		AddJobHelper<TJob, TBaseJob>(std::forward<TCreateFunc>(aCreateFunc), TupleWrapper<typename TJob::Dependencies>());
	}

	template<typename TJob>
	Result<EGetPoliciesError, std::vector<Policy>> GetJobPolicies()
	{
		auto it = myJobData.find(typeid(TJob));

		if (it == myJobData.cend())
			return EGetPoliciesError::JobNotRegistered;

		return it->second.Policies;
	}

	template<typename TJob>
	Result<ECreateError, SharedReference<TJob>, std::string> Create(SystemContainer& aSystemContainer)
	{
		auto it = myJobData.find(typeid(TJob));

		if (it == myJobData.cend())
			return ECreateError::JobNotRegistered;

		auto result = it->second.CreateFunc(aSystemContainer);

		if (!result)
			return { static_cast<ECreateError>(result), result.Meta() };

		return StaticReferenceCast<TJob>(static_cast<SharedReference<jobs_refactor::JobBase>>(result));
	}

private:
	template<
		typename TDependencyTuple>
	struct TupleWrapper {};

	struct JobData
	{
		std::function<Result<ECreateError, SharedReference<jobs_refactor::JobBase>, std::string>(SystemContainer&)> CreateFunc;
		std::vector<Policy> Policies;
	};

	template<
		typename TJob,
		typename TBaseJob,
		typename TCreateFunc,
		typename... TDependencies>
	inline void AddJobHelper(TCreateFunc&& aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&) requires(std::is_invocable_r_v<SharedReference<TJob>, TCreateFunc, std::tuple<std::decay_t<TDependencies>&...>>)
	{
		auto construct = [createFunc = std::forward<TCreateFunc>(aCreateFunc)](SystemContainer& aSystemContainer) -> Result<ECreateError, SharedReference<jobs_refactor::JobBase>, std::string>
		{
			std::optional<Result<ECreateError, SharedReference<jobs_refactor::JobBase>, std::string>> errResult;

			auto sanitizeDependency = [&errResult](auto aDependencyPtr)
			{
				if (!aDependencyPtr)
					errResult = { ECreateError::MissingDependency, typeid(std::decay_t<decltype(*aDependencyPtr)>).name() };

				return aDependencyPtr;
			};

			auto dependenyPointers = std::make_tuple(sanitizeDependency(aSystemContainer.TryGet<std::decay_t<TDependencies>>())...);

			if (errResult)
				return *errResult;

			auto ref = std::apply([&createFunc](auto... aDependenyPointers) -> SharedReference<TJob>
				{
					return createFunc(std::tie((*aDependenyPointers)...));
				}, dependenyPointers);

			SharedReference<jobs_refactor::JobBase> refBase = std::move(ref);

			return refBase;
		};

		myJobData[typeid(TBaseJob)] = { std::move(construct), TJob::PolicySet::GetPolicies() };

	}

	std::unordered_map<std::type_index, JobData> myJobData;

};
