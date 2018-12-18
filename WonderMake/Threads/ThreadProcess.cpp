#include "stdafx.h"
#include "ThreadProcess.h"

#include "Message/DataRouters.h"
#include "Message/DispatchableBuffer.h"
#include "Message/DispatchRouter.h"
#include "Message/MessageTypes.h"

ThreadProcess::ThreadProcess(const EThreadId aThreadId)
	: mySubscriber(aThreadId,
		BindHelper(&ThreadProcess::OnTask, this))
	, myRun(true)
	, myThreadId(aThreadId)
{
}

ThreadProcess::~ThreadProcess()
{
	Stop();
}

void ThreadProcess::Start(const bool aCreateThread)
{
	if (aCreateThread)
	{
		myThread = std::thread(&ThreadProcess::Run, this);
	}
	else
	{
		Run();
	}
}

void ThreadProcess::Stop()
{
	myRun = false;
	myThread.join();
}

void ThreadProcess::Procedure() {}

void ThreadProcess::Run()
{
	auto& router = DataRouters::Get().GetRouter(myThreadId);
	while (myRun)
	{
		router.CommitChanges();
		if (DispatchableBufferBase::UpdateBuffers(myThreadId))
		{
			const auto& updatedBuffers = DispatchableBufferBase::GetUpdatedBuffers(myThreadId);

			for (const auto buffer : updatedBuffers)
			{
				buffer->UpdateList();
				const auto& list = buffer->GetList();
				for (const auto dispatchable : list)
				{
					router.RouteDispatchable(*dispatchable);
				}
			}
		}

		Procedure();
	}
}

void ThreadProcess::OnTask(const Task& aTask) const
{
	aTask.Run();
}
