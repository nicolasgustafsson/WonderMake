#pragma once
#include <System/System.h>

class GlfwFacade;
struct GLFWwindow;

class Window
	: public System<
		Policy::Set<
			PAdd<GlfwFacade, PWrite>>>
{
public:
	Window();

	void Update();

	bool IsValid() const;

	GLFWwindow* myGlfwWindow = nullptr;
};
