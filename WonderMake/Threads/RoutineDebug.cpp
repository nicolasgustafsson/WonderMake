#include "stdafx.h"
#include "RoutineDebug.h"
#include "Constants.h"


RoutineDebug::RoutineDebug()
	: Routine(ERoutineId::Debug)
{
}


RoutineDebug::~RoutineDebug()
{
}

void RoutineDebug::PreMessageRouting()
{
	Routine::PreMessageRouting();

	//FinishFrame binds the backbuffer which we want to render to
	myRendererPtr->FinishFrame();
	myImguiWrapper.StartFrame();

	WmDispatchMessage<SDebugMessage>({}, ERoutineId::Debug);
}

void RoutineDebug::Procedure()
{
	Routine::Procedure();
	myImguiWrapper.EndFrame();
}
