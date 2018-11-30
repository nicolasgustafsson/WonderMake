#include "stdafx.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(const SVector2<int> Size)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	myGlfwWindow = glfwCreateWindow(Size.X, Size.Y, "WonderMake", NULL, NULL);
	if (!myGlfwWindow)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(myGlfwWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
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
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}

bool Window::ShouldClose() const
{
	return (!myGlfwWindow || glfwWindowShouldClose(myGlfwWindow));
}
