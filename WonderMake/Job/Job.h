#pragma once

#include "Typedefs.h"

#include "Policies/Policy.h"

#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <functional>
#include <mutex>
#include <optional>

enum class EJobStatus
{
	NotStarted,
	InProgress,
	Canceled,
	Succeeded,
	Failed
};
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
	using Callback = std::function<void(const EJobResult)>;

	virtual ~JobBase();

	void Start();
	void Cancel();

	[[nodiscard]] bool IsCompleted() const noexcept;
	[[nodiscard]] bool IsSuccessful() const noexcept;
	[[nodiscard]] bool IsCanceled() const noexcept;
	[[nodiscard]] EJobStatus GetStatus() const noexcept;

protected:
	template<typename TCallable>
	inline void SetCallback(TCallable aCallable) noexcept
	{
		myCallback = std::forward<TCallable>(aCallable);
	}

	void CompleteSuccess();
	void CompleteFailure();

	virtual void OnStarted() = 0;
	virtual void OnCompleted(const EJobResult /*aResult*/) {};

private:
	void SetStatus(const EJobStatus aStatus) noexcept;

	void Complete(const EJobStatus aStatus, const EJobResult aResult);
	void RunCompletedCallbacks(const EJobResult aResult);

	mutable std::mutex myLock;

	Callback myCallback;
	EJobStatus myStatus = EJobStatus::NotStarted;

};

template<
	typename TPolicySet = Policy::Set<>>
class Job
	: public JobBase
	, public std::enable_shared_from_this<Job<TPolicySet>>
{
public:
	using Super = Job<TPolicySet>;
	using PolicySet = TPolicySet;
	using Dependencies = typename TPolicySet::template DependenciesRef;

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

	template<typename TDependency> requires TPolicySet::template HasPolicy_v<TDependency, PWrite>
	__forceinline [[nodiscard]] TDependency& Get()
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires TPolicySet::template HasDependency_v<TDependency>
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
