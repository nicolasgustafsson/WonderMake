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

template<
	typename TPolicySet = Policy::Set<>>
class Job
	: NonCopyable
	, NonMovable
{
public:
	using Super = Job<TPolicySet>;
	using Dependencies = typename TPolicySet::template DependenciesRef;

	using Callback = std::function<void(const EJobResult)>;

	inline static void InjectDependencies(Dependencies&& aDependencies)
	{
		myInjectedDependencies.emplace(std::move(aDependencies));
	}

	inline virtual ~Job()
	{
		Cancel();
	}

	inline void Start()
	{
		{
			std::lock_guard<decltype(myLock)> lock(myLock);

			if (myStatus != EJobStatus::NotStarted)
			{
				return;
			}

			myStatus = EJobStatus::InProgress;
		}

		OnStarted();
	}
	inline void Cancel()
	{
		Complete(EJobStatus::Canceled, EJobResult::Canceled);
	}

	inline [[nodiscard]] bool IsCompleted() const noexcept
	{
		const auto status = GetStatus();

		return status != EJobStatus::NotStarted
			&& status != EJobStatus::InProgress;
	}
	inline [[nodiscard]] bool IsSuccessful() const noexcept
	{
		return GetStatus() == EJobStatus::Succeeded;
	}
	inline [[nodiscard]] bool IsCanceled() const noexcept
	{
		return GetStatus() == EJobStatus::Canceled;
	}
	inline [[nodiscard]] EJobStatus GetStatus() const noexcept
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		return myStatus;
	}

protected:
	inline Job() noexcept
		: myDependencies(std::move(*myInjectedDependencies))
	{
		myInjectedDependencies.reset();
	}

	template<typename TDependency> requires
		TPolicySet::template HasPolicy_v<TDependency, PWrite>
	__forceinline TDependency& Get()
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TDependency> requires
		TPolicySet::template HasDependency_v<TDependency>
	__forceinline const TDependency& Get() const
	{
		return std::get<std::decay_t<TDependency>&>(myDependencies);
	}

	template<typename TCallable>
	inline void SetCallback(TCallable aCallable) noexcept
	{
		myCallback = std::forward<TCallable>(aCallable);
	}
	
	inline void CompleteSuccess()
	{
		Complete(EJobStatus::Succeeded, EJobResult::Succeeded);
	}
	inline void CompleteFailure()
	{
		Complete(EJobStatus::Failed, EJobResult::Failed);
	}

	inline virtual void OnStarted() = 0;
	inline virtual void OnCompleted(const EJobResult /*aResult*/) {};

private:
	inline void SetStatus(const EJobStatus aStatus) noexcept
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		myStatus = aStatus;
	}

	inline void Complete(const EJobStatus aStatus, const EJobResult aResult)
	{
		{
			std::lock_guard<decltype(myLock)> lock(myLock);

			if (myStatus != EJobStatus::InProgress)
			{
				return;
			}

			myStatus = aStatus;
		}

		RunCompletedCallbacks(aResult);
	}
	inline void RunCompletedCallbacks(const EJobResult aResult)
	{
		OnCompleted(aResult);

		Utility::Invoke(myCallback, aResult);
	}

	static thread_local std::optional<Dependencies> myInjectedDependencies;

	mutable std::mutex myLock;

	Dependencies myDependencies;
	Callback myCallback;
	EJobStatus myStatus = EJobStatus::NotStarted;
};

template<typename TPolicySet>
thread_local std::optional<typename Job<TPolicySet>::Dependencies> Job<TPolicySet>::myInjectedDependencies;
