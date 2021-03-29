#include "pch.h"
#include "Routine.h"

#include "Message/DispatchRouter.h"

#include "Graphics/Renderer.h"

#include "Program/ImguiWrapper.h"

void Routine::Run()
{
	RouteMessages();

	if constexpr (Constants::EnableImGui)
	{
		SystemPtr<Renderer>()->FinishFrame();
		SystemPtr<ImguiWrapper>()->StartFrame();

		auto&& router = DispatchRouter::Get();

		router.RouteDispatchable(SDebugMessage());

		router.CommitChanges();

		SystemPtr<ImguiWrapper>()->EndFrame();
	}

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
