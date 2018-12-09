#include "stdafx.h"
#include "ThreadProcess.h"

#include "Message/DataRouters.h"
#include "Message/DispatchRouter.h"

ThreadProcess::ThreadProcess(const EThreadId ThreadId)
	: myRun(true)
	, myThreadId(ThreadId)
{
	myThread = std::thread(&ThreadProcess::Run, this);
}

ThreadProcess::~ThreadProcess()
{
	myRun = false;
	myThread.join();
}

void ThreadProcess::Stop()
{
	myRun = false;
}

void ThreadProcess::Procedure() {}

void ThreadProcess::Run()
{
	while (myRun)
	{
		DataRouters::Get().GetRouter(myThreadId).FlushMessagesAndRunTasks();
		Procedure();
	}
}
