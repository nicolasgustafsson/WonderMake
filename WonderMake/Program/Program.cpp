#include "stdafx.h"
#include "Program.h"
#include <GLFW/glfw3.h>
#include <functional>
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_glfw.h"
#include "../Imgui/imgui_impl_opengl3.h"
#include "../Graphics/Shader.h"
#include <iostream>

Program::Program()
	:myWindow(WindowSize), myRenderer(), myImguiWrapper(myWindow)
{
	SetupCallbacks();
}

void Program::Start()
{
	while (!myWindow.ShouldClose())
	{
		Update();
	}
}

void Program::Update()
{
	myWindow.Update();

	myRenderer.SwapFrame();

	myImguiWrapper.StartFrame();

	ImGui::ShowDemoWindow();

	myImguiWrapper.EndFrame();
}

void Program::SetupCallbacks()
{
	//sets the user pointer so we can access ourself from the lambda
	glfwSetWindowUserPointer(myWindow.myGlfwWindow, this);

	glfwSetFramebufferSizeCallback(myWindow.myGlfwWindow, [](GLFWwindow* Window, int X, int Y) -> void
	{
		Program* SelfPointer = static_cast<Program*>(glfwGetWindowUserPointer(Window));
		SelfPointer->OnWindowSizeChanged(Window, X, Y);
	});
}

void Program::OnWindowSizeChanged(GLFWwindow* Window, i32 X, i32 Y)
{
	myRenderer.SetViewportSize({ X, Y });
}
