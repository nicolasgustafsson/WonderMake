#pragma once

#include "Typedefs.h"

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

	inline EJobResult GetResult() const;

protected:
	inline void Reset();
	inline void Complete(const EJobResult aResult);

	Closure myCallback;

private:
	EJobResult myResult = EJobResult::InProgress;
};

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
