#pragma once

#include "System/System.h"

class GlfwFacade;
class Window;

class ImguiWrapper
	: public System<
		Policy::Set<
			Policy::Add<GlfwFacade, Policy::EPermission::Write>,
			Policy::Add<Window, Policy::EPermission::Read>>>
{
public:
	ImguiWrapper(Dependencies&& aDependencies);

	void StartFrame();

	void EndFrame();
};

