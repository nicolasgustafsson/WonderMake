#pragma once

#include "wondermake-base/System.h"

#include <string>

class GlfwFacade;
class Window;

class ConfigurationSystem;

class ImguiWrapper
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<Window, PRead>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	ImguiWrapper();

	void StartFrame();

	void EndFrame();

private:
	void DockSpace();

	std::string myIniFileLocation;

};