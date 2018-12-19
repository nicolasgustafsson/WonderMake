#include "stdafx.h"
#include "Routine.h"

#include "Message/DataRouters.h"

Routine::Routine(const ERoutineId aRoutineId)
	: myRoutineId(aRoutineId)
	, mySubscriber(aRoutineId,
		BindHelper(&Routine::OnTask, this))
{}

void Routine::Run()
{
	RouteMessages();
	Procedure();
}

void Routine::Procedure() {}

void Routine::RouteMessages()
{
	auto& router = DataRouters::Get().GetRouter(myRoutineId);
	router.CommitChanges();
	if (DispatchableBufferBase::UpdateBuffers(myRoutineId))
	{
		const auto& updatedBuffers = DispatchableBufferBase::GetUpdatedBuffers(myRoutineId);

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
}

void Routine::OnTask(const Task& aTask) const
{
	aTask.Run();
}
