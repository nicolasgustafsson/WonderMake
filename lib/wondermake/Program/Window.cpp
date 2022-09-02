#include "pch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Json/json.hpp"
#include <fstream>
#include "Program/GlfwFacade.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/WmLogTags.h"

REGISTER_SYSTEM(Window);

Window::Window()
{
	auto&& configuration = Get<ConfigurationSystem>();
	
	auto& glfw = Get<GlfwFacade>();
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfw.SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mySize =
	{
		configuration.Get<i32>(ConfigurationEngine::WindowWidth, 1280),
		configuration.Get<i32>(ConfigurationEngine::WindowHeight, 720)
	};

	myGlfwWindow = glfw.CreateGlfwWindow(mySize.X, mySize.Y, "WonderMake", NULL, NULL);
	if (!myGlfwWindow)
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Failed to create GLFW window.");
		glfw.Terminate();
		return;
	}

	glfw.MakeContextCurrent(myGlfwWindow);

	if (!glfw.InitializeGlad())
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Failed to initialize GLAD.");
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
		quick_exit(0);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}

SVector2i Window::GetSize() const
{
	return mySize;
}
