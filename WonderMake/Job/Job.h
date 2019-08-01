#pragma once

#include "Typedefs.h"

#include "Message/Messages.h"

#include "Threads/RoutineIds.h"

#include <functional>
#include <mutex>

class Job
{
public:
	enum class EResult
	{
		InProgress,
		Cancelled,
		Success,
		Failure
	};

	using Callback = std::function<void(const EResult)>;

	Job() noexcept = default;
	Job(const Job&) noexcept = default;
	inline virtual ~Job() noexcept;

	Job& operator =(const Job&) noexcept = default;

	inline void Cancel() noexcept;

	inline bool IsComplete() noexcept;
	inline bool IsSuccessful() noexcept;
	inline EResult GetResult() noexcept;

protected:
	inline void Reset() noexcept;
	inline void Complete(const EResult aResult) noexcept;
	inline void CompleteOnRoutine(const EResult aResult, const ERoutineId aRoutineId) noexcept;

	inline virtual void OnReset() noexcept {};
	inline virtual void OnCancel() noexcept {};
	inline virtual void OnComplete(const EResult /*aResult*/) noexcept {};

	Callback myCallback;

private:
	inline void SetResult(const EResult aResult) noexcept;

	std::mutex myLock;
	EResult myResult = EResult::InProgress;
};

inline Job::~Job() noexcept
{
	Cancel();
}

inline void Job::Cancel() noexcept
{
	{
		std::lock_guard<decltype(myLock)> lock(myLock);
		if (myResult != EResult::InProgress)
		{
			return;
		}
		myResult = EResult::Cancelled;
	}
	OnCancel();
}

inline bool Job::IsComplete() noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult != EResult::InProgress;
}

inline bool Job::IsSuccessful() noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult == EResult::Success;
}

inline Job::EResult Job::GetResult() noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult;
}

inline void Job::Reset() noexcept
{
	SetResult(EResult::InProgress);
	OnReset();
}

inline void Job::Complete(const EResult aResult) noexcept
{
	SetResult(aResult);
	OnComplete(aResult);
	if (myCallback)
	{
		myCallback(aResult);
	}
}

inline void Job::CompleteOnRoutine(const EResult aResult, const ERoutineId aRoutineId) noexcept
{
	SetResult(aResult);
	OnComplete(aResult);
	if (myCallback)
	{
		WmDispatchTask([&, aResult]
			{
				if (!myCallback)
				{
					return;
				}
				myCallback(aResult);
			}, aRoutineId);
	}
}

inline void Job::SetResult(const EResult aResult) noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myResult = aResult;
}