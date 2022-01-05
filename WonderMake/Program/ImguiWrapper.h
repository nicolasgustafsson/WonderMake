#pragma once

#include "System/System.h"

class GlfwFacade;
class Window;

class ImguiWrapper
	: public System<
		Policy::Set<
			PAdd<GlfwFacade, PWrite>,
			PAdd<Window, PRead>>,
		STrait::Set<
			STGui>>
{
public:
	ImguiWrapper();

	void StartFrame();

	void EndFrame();
};

