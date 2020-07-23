#pragma once
#include <System/System.h>

class GlfwFacade;
struct GLFWwindow;

class Window
	: public System<
		Policy::Set<
			Policy::Add<GlfwFacade, Policy::EPermission::Write>>>
{
public:
	Window(Dependencies&& aDependencies);

	void Update();

	bool IsValid() const;

	GLFWwindow* myGlfwWindow = nullptr;
};

REGISTER_SYSTEM(Window);
