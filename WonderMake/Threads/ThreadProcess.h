#pragma once

#include "ThreadIds.h"

#include "Utilities/RestrictTypes.h"

#include <atomic>
#include <thread>

class ThreadProcess
	: private NonCopyable
{
public:
	ThreadProcess(const EThreadId ThreadId);
	virtual ~ThreadProcess();

	void Stop();

protected:
	virtual void Procedure();

	const EThreadId myThreadId;

private:
	void Run();

	std::atomic_bool myRun;
	std::thread myThread;
};
