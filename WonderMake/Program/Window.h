#pragma once
#include <System/System.h>

class GlfwFacade;
struct GLFWwindow;

class Window
	: public Systemus<Window, GlfwFacade>
{
public:
	Window();

	void Update();

	bool IsValid() const;

	GLFWwindow* myGlfwWindow = nullptr;
};
