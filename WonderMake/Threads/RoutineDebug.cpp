#include "pch.h"
#include "RoutineDebug.h"
#include "Constants.h"


RoutineDebug::RoutineDebug()
	: Routine(ERoutineId::Debug)
{
}

void RoutineDebug::PreMessageRouting()
{
	Routine::PreMessageRouting();

	//FinishFrame binds the backbuffer which we want to render to
	SystemPtr<Renderer>()->FinishFrame();
	SystemPtr<ImguiWrapper>()->StartFrame();

	WmDispatchMessage<SDebugMessage>({}, ERoutineId::Debug);
}

void RoutineDebug::Procedure()
{
	Routine::Procedure();
	SystemPtr<ImguiWrapper>()->EndFrame();
}
