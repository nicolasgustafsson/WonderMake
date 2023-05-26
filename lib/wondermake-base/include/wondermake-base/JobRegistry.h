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

	struct SError
	{
		ECreateError	Error = ECreateError::JobNotRegistered;
		std::string		Reason;
	};

	template<
		CJob TJob,
		typename TBaseJob = TJob,
		typename TCreateFunc>
	inline void AddJob(TCreateFunc&& aCreateFunc)
		requires(std::is_copy_constructible_v<TCreateFunc>
			&& std::is_base_of_v<TBaseJob, TJob>)
	{
		AddJobHelper<TJob, TBaseJob>(std::forward<TCreateFunc>(aCreateFunc), TupleWrapper<typename TJob::Dependencies>());
	}

	template<typename TJob>
	Result<std::vector<Policy>, EGetPoliciesError> GetJobPolicies() const
	{
		auto it = myJobData.find(typeid(TJob));

		if (it == myJobData.cend())
			return Err(EGetPoliciesError::JobNotRegistered);

		return Ok(it->second.Policies);
	}

	template<typename TJob>
	Result<SharedReference<TJob>, SError> Create(SystemContainer& aSystemContainer) const
	{
		auto it = myJobData.find(typeid(TJob));

		if (it == myJobData.cend())
			return Err(SError{ ECreateError::JobNotRegistered });

		auto result = it->second.CreateFunc(aSystemContainer);

		if (!result)
			return result;

		return Ok(StaticReferenceCast<TJob>(result.Unwrap()));
	}

private:
	template<
		typename TDependencyTuple>
	struct TupleWrapper {};

	struct JobData
	{
		std::function<Result<SharedReference<JobBase>, SError>(SystemContainer&)> CreateFunc;
		std::vector<Policy> Policies;
	};

	template<
		typename TJob,
		typename TBaseJob,
		typename TCreateFunc,
		typename... TDependencies>
	inline void AddJobHelper(TCreateFunc&& aCreateFunc, TupleWrapper<std::tuple<SharedReference<TDependencies>...>>&&) requires(std::is_invocable_r_v<SharedReference<TJob>, TCreateFunc, std::tuple<SharedReference<std::decay_t<TDependencies>>&...>>)
	{
		auto construct = [createFunc = std::forward<TCreateFunc>(aCreateFunc)]([[maybe_unused]] SystemContainer& aSystemContainer) -> Result<SharedReference<JobBase>, SError>
		{
			std::optional<Result<SharedReference<JobBase>, SError>> errResult;

			auto sanitizeDependency = [&errResult](auto aDependencyPtr)
			{
				auto result = SharedReference<std::decay_t<decltype(*aDependencyPtr)>>::FromPointer(std::move(aDependencyPtr));

				if (!result)
					errResult = Err(SError{ ECreateError::MissingDependency, typeid(std::decay_t<decltype(*aDependencyPtr)>).name() });

				return result;
			};

			auto dependenyResults = std::make_tuple(sanitizeDependency(aSystemContainer.GetPtr<std::decay_t<TDependencies>>())...);

			if (errResult)
				return *errResult;

			auto ref = std::apply([&createFunc](auto... aResult) -> SharedReference<TJob>
				{
					return std::move(createFunc)(std::make_tuple((aResult.Unwrap())...));
				}, dependenyResults);

			SharedReference<JobBase> refBase = std::move(ref);

			return Ok(std::move(refBase));
		};

		myJobData[typeid(TBaseJob)] = { std::move(construct), TJob::PolicySet::GetPolicies() };

	}

	std::unordered_map<std::type_index, JobData> myJobData;

};
