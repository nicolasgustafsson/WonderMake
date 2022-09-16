#pragma once

#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"

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

	virtual EState GetState() const = 0;
	
	virtual void Terminate(i64 aExitCode) = 0;

	virtual Future<Result<i64, SExitError>> OnExit() = 0;

};
