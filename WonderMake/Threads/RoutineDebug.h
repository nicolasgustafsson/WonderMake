#pragma once
#include "Program/ImguiWrapper.h"
#include "Graphics/Renderer.h"
#include "System/SystemPtr.h"
#include "Routine.h"

class RoutineDebug
	: public Routine
{
public:
	RoutineDebug();
	~RoutineDebug();

protected:
	virtual void PreMessageRouting() override;
	virtual void Procedure() override;
private:
	ImguiWrapper myImguiWrapper;

	SystemPtr<Renderer> myRendererPtr;
};

