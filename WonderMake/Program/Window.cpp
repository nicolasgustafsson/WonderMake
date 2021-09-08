#include "pch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Json/json.hpp"
#include <fstream>
#include "Program/GlfwFacade.h"
#include "Camera/CameraManager.h"

REGISTER_SYSTEM(Window);

Window::Window()
{
	std::ifstream windowSettingsFile("windowSettings.json");
	json windowSettings;

	windowSettingsFile >> windowSettings;

	auto& glfw = Get<GlfwFacade>();
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfw.SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfw.SetWindowHint(GLFW_SAMPLES, 4);

	auto* primaryMonitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	const bool fullscreen = windowSettings["Fullscreen"].get<bool>();

	SVector2i windowSize{};

	if (!fullscreen)
		windowSize = { windowSettings["X"].get<i32>(), windowSettings["Y"].get<i32>() };
	else
		windowSize = { mode->width, mode->height };

	myGlfwWindow = glfw.CreateGlfwWindow(windowSize.X, windowSize.Y, "WonderMake", windowSettings["Fullscreen"].get<bool>() ? primaryMonitor : NULL, NULL);
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

	if constexpr (!Constants::IsDebugging)
		SystemPtr<CameraManager>()->SetViewportSize(windowSize);
}

void Window::Update()
{
	auto& glfw = Get<GlfwFacade>();

	if (glfw.GetKey(myGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfw.SetWindowShouldClose(myGlfwWindow, true);

	glfw.PollEvents();

	if (glfw.ShouldWindowClose(myGlfwWindow))
	{
		quick_exit(0);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}