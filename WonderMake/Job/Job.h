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

	inline virtual ~Job();

	inline void Cancel();

	inline bool IsComplete();
	inline bool IsSuccessful();
	inline EResult GetResult();

protected:
	inline void Reset();
	inline void Complete(const EResult aResult);
	inline void CompleteOnRoutine(const EResult aResult, const ERoutineId aRoutineId);

	inline virtual void OnReset() {};
	inline virtual void OnCancel() {};
	inline virtual void OnComplete(const EResult /*aResult*/) {};

	Callback myCallback;

private:
	inline void SetResult(const EResult aResult);

	std::mutex myLock;
	EResult myResult = EResult::InProgress;
};

inline Job::~Job()
{
	Cancel();
}

inline void Job::Cancel()
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

inline bool Job::IsComplete()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult != EResult::InProgress;
}

inline bool Job::IsSuccessful()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult == EResult::Success;
}

inline Job::EResult Job::GetResult()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	return myResult;
}

inline void Job::Reset()
{
	SetResult(EResult::InProgress);
	OnReset();
}

inline void Job::Complete(const EResult aResult)
{
	SetResult(aResult);
	OnComplete(aResult);
	if (myCallback)
	{
		myCallback(aResult);
	}
}

inline void Job::CompleteOnRoutine(const EResult aResult, const ERoutineId aRoutineId)
{
	SetResult(aResult);
	OnComplete(aResult);
	if (myCallback)
	{
		WmDispatchTask([&, aResult] { myCallback(aResult); }, aRoutineId);
	}
}

inline void Job::SetResult(const EResult aResult)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myResult = aResult;
}