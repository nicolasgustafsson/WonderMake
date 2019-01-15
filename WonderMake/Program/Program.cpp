#include "stdafx.h"
#include "Program.h"
#include <GLFW/glfw3.h>
#include <functional>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Graphics/Shader.h"
#include <iostream>
#include "../Threads/DataThreads.h"
#include "Camera/Camera.h"
#include "Graphics/EngineUniformBuffer.h"

Program::Program()
	:myWindow(WindowSize), myRenderer(), myImguiWrapper(myWindow)
{
	SetupCallbacks();
}

void Program::Update()
{
	EngineUniformBuffer::Get().GetBuffer().Time += 0.01f;

	myWindow.Update();

	Camera::Get().Update();

	myRenderer.SwapFrame();

	if constexpr (Constants::IsDebugging)
	{
		myImguiWrapper.StartFrame();

		myDockSpace.Debug();

		ImGui::ShowDemoWindow();
		myImGuiLogger.Draw();
		myRenderer.Debug();
		Camera::Get().Debug();
		EngineUniformBuffer::Get().Debug();

		myImguiWrapper.EndFrame();
	}
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

void Program::OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y)
{
	myRenderer.SetViewportSize({ X, Y });
}

