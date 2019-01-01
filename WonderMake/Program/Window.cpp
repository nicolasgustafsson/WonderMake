#include "stdafx.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(const SVector2<int> Size)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	myGlfwWindow = glfwCreateWindow(Size.X, Size.Y, "WonderMake", NULL, NULL);
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

	glfwSwapBuffers(myGlfwWindow);
	glfwPollEvents();

	if (glfwWindowShouldClose(myGlfwWindow))
	{
		WmDispatchTask([]() {quick_exit(0);}, EThreadId::Logic);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}