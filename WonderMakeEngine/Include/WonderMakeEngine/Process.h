#pragma once

#include "WonderMakeBase/Result.h"
#include "WonderMakeBase/UniqueFunction.h"

class Process
{
public:
	enum class EState
	{
		Stopped,
		Running
	};
	enum class EExitError
	{
		AlreadyStopped,
		Terminated,
		InternalError
	};

	using OnExitCallback = UniqueFunction<void(Result<EExitError, i64, i64>)>;

	virtual EState GetState() const = 0;
	
	virtual void Terminate(i64 aExitCode) = 0;

	virtual void OnExit(OnExitCallback&& aOnExit) = 0;

};
