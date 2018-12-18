#pragma once

#include "ThreadIds.h"

#include "Message/MessageSubscriber.h"

#include "Utilities/RestrictTypes.h"

#include <atomic>
#include <thread>

class Task;

class ThreadProcess
	: private NonCopyable
{
public:
	ThreadProcess(const EThreadId aThreadId);
	virtual ~ThreadProcess();

	void Start(const bool aCreateThread = true);
	void Stop();

protected:
	virtual void Procedure();

	const EThreadId myThreadId;

private:
	void Run();
	
	void OnTask(const Task& aTask) const;

	MessageSubscriber mySubscriber;
	std::atomic_bool myRun;
	std::thread myThread;
};
