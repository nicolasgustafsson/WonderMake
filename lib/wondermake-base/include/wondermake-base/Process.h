#pragma once

#include "wondermake-utility/Result.h"
#include "wondermake-utility/UniqueFunction.h"

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

	struct SExitError
	{
		EExitError Error = EExitError::InternalError;
		i64 Reason = 0;
	};

	using OnExitCallback = UniqueFunction<void(Result<i64, SExitError>)>;

	virtual EState GetState() const = 0;
	
	virtual void Terminate(i64 aExitCode) = 0;

	virtual void OnExit(OnExitCallback&& aOnExit) = 0;

};
