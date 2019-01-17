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

void Routine::AddProcedure(Closure aClosure)
{
	myProcedures.push_back(aClosure);
}

void Routine::Procedure() 
{
	for(auto&& procedure : myProcedures)
		procedure();
}

void Routine::RouteMessages()
{
	auto& router = DataRouters::Get().GetRouter(myRoutineId);
	router.CommitChanges();

	std::vector<const Dispatchable*> dispatchableList;

	if (DispatchableBufferBase::UpdateBuffers(myRoutineId))
	{
		DispatchableBufferBase::GetDispatchableList(myRoutineId, dispatchableList);
		for (const auto dispatchable : dispatchableList)
		{
			router.RouteDispatchable(*dispatchable);
		}
	}
}

void Routine::OnTask(const Task& aTask) const
{
	aTask.Run();
}
