#include "pch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Json/json.hpp"
#include <fstream>

Window::Window()
{
	std::ifstream windowSettingsFile("windowSettings.json");
	json windowSettings;

	windowSettingsFile >> windowSettings;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	myGlfwWindow = glfwCreateWindow(windowSettings["X"].get<i32>(), windowSettings["Y"].get<i32>(), "WonderMake", NULL, NULL);
	if (!myGlfwWindow)
	{
		WmLog(TagError, TagOpenGL, "Failed to create GLFW window!");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(myGlfwWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		WmLog(TagError, TagOpenGL, "Failed to initialize GLAD");
	}
}

Window::~Window()
{
}

void Window::Update()
{
	if (glfwGetKey(myGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(myGlfwWindow, true);

	glfwPollEvents();

	if (glfwWindowShouldClose(myGlfwWindow))
	{
		WmDispatchTask([]() {quick_exit(0);}, ERoutineId::Logic);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}