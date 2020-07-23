#include "pch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Json/json.hpp"
#include <fstream>
#include "Program/GlfwFacade.h"

Window::Window(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
{
	std::ifstream windowSettingsFile("windowSettings.json");
	json windowSettings;

	windowSettingsFile >> windowSettings;

	auto& glfw = Get<GlfwFacade>();
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfw.SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	myGlfwWindow = glfw.CreateGlfwWindow(windowSettings["X"].get<i32>(), windowSettings["Y"].get<i32>(), "WonderMake", NULL, NULL);
	if (!myGlfwWindow)
	{
		WmLog(TagError, TagOpenGL, "Failed to create GLFW window!");
		glfw.Terminate();
		return;
	}

	glfw.MakeContextCurrent(myGlfwWindow);

	if (!glfw.InitializeGlad())
	{
		WmLog(TagError, TagOpenGL, "Failed to initialize GLAD");
	}
}

void Window::Update()
{
	auto& glfw = Get<GlfwFacade>();

	if (glfw.GetKey(myGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfw.SetWindowShouldClose(myGlfwWindow, true);

	glfw.PollEvents();

	if (glfw.ShouldWindowClose(myGlfwWindow))
	{
		WmDispatchTask([]() {quick_exit(0);}, ERoutineId::Logic);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}