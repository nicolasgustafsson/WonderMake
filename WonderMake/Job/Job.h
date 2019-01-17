#pragma once

#include "Typedefs.h"

#include "Message/Messages.h"

#include "Threads/RoutineIds.h"

#include <functional>

enum class EJobResult
{
	InProgress,
	Success,
	Failure
};

class Job
{
public:
	virtual ~Job() = default;

	inline bool IsComplete() const;
	inline EJobResult GetResult() const;

protected:
	inline void Reset();
	inline void Complete(const EJobResult aResult);
	inline void CompleteOnRoutine(const EJobResult aResult, const ERoutineId aRoutineId);

	Closure myCallback;

private:
	EJobResult myResult = EJobResult::InProgress;
};

inline bool Job::IsComplete() const
{
	return myResult == EJobResult::Success;
}

inline EJobResult Job::GetResult() const
{
	return myResult;
}

inline void Job::Reset()
{
	myResult = EJobResult::InProgress;
}

inline void Job::Complete(const EJobResult aResult)
{
	myResult = aResult;
	if (myCallback)
	{
		myCallback();
	}
}

inline void Job::CompleteOnRoutine(const EJobResult aResult, const ERoutineId aRoutineId)
{
	myResult = aResult;
	if (myCallback)
	{
		WmDispatchTask(myCallback, aRoutineId);
	}
}