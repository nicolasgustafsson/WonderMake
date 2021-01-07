#pragma once

#include "Typedefs.h"

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <optional>

enum class EJobResult
{
	Canceled,
	Succeeded,
	Failed
};

class JobBase
	: NonCopyable
	, NonMovable
{
public:
	virtual ~JobBase();

	void Cancel();

protected:
	void CompleteSuccess();
	void CompleteFailure();

	virtual void OnCompleted(const EJobResult /*aResult*/) {};

private:
	void Complete(const EJobResult aResult);

};

class JobSystem;

template<
	typename TPolicySet = Policy::Set<>>
class Job
	: public JobBase
	, public std::enable_shared_from_this<Job<TPolicySet>>
{
public:
	using Super = Job<TPolicySet>;
	using PolicySet = Policy::Concat<
		Policy::Set<
			PAdd<JobSystem, PWrite>>,
		TPolicySet>;

	using Dependencies = typename PolicySet::template DependenciesRef;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

protected:
	inline Job() noexcept
		: myDependencies(std::move(*myInjectedDependencies))
	{
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

private:
	static thread_local std::optional<Dependencies> myInjectedDependencies;

	Dependencies myDependencies;

};

template<typename TPolicySet>
thread_local std::optional<typename Job<TPolicySet>::Dependencies> Job<TPolicySet>::myInjectedDependencies;
