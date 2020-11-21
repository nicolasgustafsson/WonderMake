#include "pch.h"
#include "Routine.h"

#include "Message/DispatchRouter.h"

#include "Graphics/Renderer.h"

#include "Program/ImguiWrapper.h"

Routine::Routine()
	: mySubscriber(BindHelper(&Routine::OnTask, this))
{}

void Routine::Run()
{
	PreMessageRouting();
	RouteMessages();
	Procedure();

	if constexpr (Constants::IsDebugging)
	{
		SystemPtr<Renderer>()->FinishFrame();
		SystemPtr<ImguiWrapper>()->StartFrame();

		auto&& router = DispatchRouter::Get();

		router.RouteDispatchable(SDebugMessage());

		router.CommitChanges();

		SystemPtr<ImguiWrapper>()->EndFrame();
	}

}

void Routine::AddProcedure(Closure aClosure)
{
	myProceduresToAdd.emplace(std::move(aClosure));
}

void Routine::Procedure() 
{
	if (myProceduresToAdd.size() > 0)
	{
		myProcedures.splice(myProceduresToAdd);

		myProceduresToAdd.clear();
	}

	for(auto&& procedure : myProcedures)
		procedure();
}

void Routine::RouteMessages()
{
	auto&& router = DispatchRouter::Get();
	
	const auto messageList = std::move(DispatchableBuffer::Get().myList);

	for (const auto& message : messageList)
	{
		router.RouteDispatchable(*message);

		router.CommitChanges();
	}
}

void Routine::OnTask(const Task& aTask) const
{
	aTask.Run();
}
