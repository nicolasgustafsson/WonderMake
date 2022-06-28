#pragma once
#include "wondermake-base/System.h"

class ConfigurationSystem;
class GlfwFacade;
struct GLFWwindow;

class Window
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>,
			PAdd<GlfwFacade, PWrite>>,
		STrait::Set<
			STGui>>
{
public:
	Window();

	void Update();

	bool IsValid() const;

	SVector2i GetSize() const;

	GLFWwindow* myGlfwWindow = nullptr;
private:
	SVector2i mySize;

};
