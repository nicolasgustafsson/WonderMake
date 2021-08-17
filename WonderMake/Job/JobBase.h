#pragma once

#include "Typedefs.h"

#include "Job/JobOutput.h"
#include "Job/JobPromise.h"

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"

#include <optional>
#include <type_traits>

enum class EJobResult
{
	Canceled,
	Succeeded,
	Failed
};

class JobSystem;

class JobBase
	: NonCopyable
	, NonMovable
{
public:
	virtual ~JobBase();

	void Cancel();

protected:
	virtual void OnCompleted(const EJobResult /*aResult*/) {};

};

template<
	typename TPolicySet = Policy::Set<>,
	typename TOutput = JobOutput<>,
	typename TOutputError = JobOutputError<>>
class Job
	: public JobBase
{
public:
	using Super = Job<TPolicySet, TOutput, TOutputError>;
	using PolicySet = Policy::Concat<
		Policy::Set<
			PAdd<JobSystem, PWrite>>,
		TPolicySet>;
	using Output = TOutput;
	using OutputError = TOutputError;

	using Promise = JobPromise<Output, OutputError>;
	using Dependencies = typename PolicySet::template DependenciesRef;

	inline static void InjectDependencies(Promise& aPromise, Dependencies&& aDependencies)
	{
		myInjectedPromise = &aPromise;
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

protected:
	inline Job() noexcept
		: myPromise(*myInjectedPromise)
		, myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedPromise = nullptr;
		myInjectedDependencies.reset();
	}

	template<typename TDependency> requires PolicySet::template HasPolicy_v<TDependency, PWrite>
	__forceinline [[nodiscard]] TDependency& Get()
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires PolicySet::template HasDependency_v<TDependency>
	__forceinline [[nodiscard]] const TDependency& Get() const
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename... TArgs>
	inline void CompleteSuccess(TArgs... aArgs)
	{
		OnCompleted(EJobResult::Succeeded);

		myPromise.Fullfill(std::forward<TArgs>(aArgs)...);
	}
	template<typename... TArgs>
	inline void CompleteFailure(TArgs... aArgs)
	{
		OnCompleted(EJobResult::Failed);

		myPromise.Fail(std::forward<TArgs>(aArgs)...);
	}

private:
	friend JobSystem;

	static thread_local Promise*					myInjectedPromise;
	static thread_local std::optional<Dependencies>	myInjectedDependencies;

	Promise& myPromise;
	Dependencies myDependencies;

};

template<typename TPolicySet, typename TOutput, typename TOutputError>
thread_local typename Job<TPolicySet, TOutput, TOutputError>::Promise* Job<TPolicySet, TOutput, TOutputError>::myInjectedPromise;

template<typename TPolicySet, typename TOutput, typename TOutputError>
thread_local std::optional<typename Job<TPolicySet, TOutput, TOutputError>::Dependencies> Job<TPolicySet, TOutput, TOutputError>::myInjectedDependencies;
