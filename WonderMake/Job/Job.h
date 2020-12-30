#pragma once

#include "Typedefs.h"

#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <functional>
#include <mutex>

class Job
	: NonCopyable
	, NonMovable
{
public:
	enum class EStatus
	{
		NotStarted,
		InProgress,
		Canceled,
		Succeeded,
		Failed
	};
	enum class EResult
	{
		Canceled,
		Succeeded,
		Failed
	};

	using Callback = std::function<void(const EResult)>;

	inline virtual ~Job()
	{
		Cancel();
	}

	inline void Start()
	{
		{
			std::lock_guard<decltype(myLock)> lock(myLock);

			if (myStatus != EStatus::NotStarted)
			{
				return;
			}

			myStatus = EStatus::InProgress;
		}

		OnStarted();
	}
	inline void Cancel()
	{
		Complete(EStatus::Canceled, EResult::Canceled);
	}

	inline [[nodiscard]] bool IsCompleted() const noexcept
	{
		const auto status = GetStatus();

		return status != EStatus::NotStarted
			&& status != EStatus::InProgress;
	}
	inline [[nodiscard]] bool IsSuccessful() const noexcept
	{
		return GetStatus() == EStatus::Succeeded;
	}
	inline [[nodiscard]] bool IsCanceled() const noexcept
	{
		return GetStatus() == EStatus::Canceled;
	}
	inline [[nodiscard]] EStatus GetStatus() const noexcept
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		return myStatus;
	}

protected:
	template<typename TCallable>
	inline void SetCallback(TCallable aCallable) noexcept
	{
		myCallback = std::forward<TCallable>(aCallable);
	}
	
	inline void CompleteSuccess()
	{
		Complete(EStatus::Succeeded, EResult::Succeeded);
	}
	inline void CompleteFailure()
	{
		Complete(EStatus::Failed, EResult::Failed);
	}

	inline virtual void OnStarted() = 0;
	inline virtual void OnCompleted(const EResult /*aResult*/) {};

private:
	inline void SetStatus(const EStatus aStatus) noexcept
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		myStatus = aStatus;
	}

	inline void Complete(const EStatus aStatus, const EResult aResult)
	{
		{
			std::lock_guard<decltype(myLock)> lock(myLock);

			if (myStatus != EStatus::InProgress)
			{
				return;
			}

			myStatus = aStatus;
		}

		RunCompletedCallbacks(aResult);
	}
	inline void RunCompletedCallbacks(const EResult aResult)
	{
		OnCompleted(aResult);

		Utility::Invoke(myCallback, aResult);
	}

	mutable std::mutex myLock;

	Callback myCallback;
	EStatus myStatus = EStatus::NotStarted;
};
