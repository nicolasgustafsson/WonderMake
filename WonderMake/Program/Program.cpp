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
{
	SetupCallbacks();
}

void Program::Update()
{
	FinishPreviousFrame();

	StartNewFrame();
}

void Program::StartNewFrame()
{
	myEngineUniformBufferPtr->GetBuffer().Time += 0.01f;

	myWindowPtr->Update();

	myCameraPtr->Update();

	myRendererPtr->StartFrame();
}

void Program::FinishPreviousFrame()
{
	myRendererPtr->FinishFrame();

	if constexpr (Constants::IsDebugging)
	{
		myImguiWrapper.StartFrame();

		myDockSpace.Debug();

		ImGui::ShowDemoWindow();
		myImGuiLogger.Draw();
		myRendererPtr->Debug();
		myCameraPtr->Debug();
		myEngineUniformBufferPtr->Debug();
		myImguiWrapper.EndFrame();
	}
}

void Program::SetupCallbacks()
{
	//sets the user pointer so we can access ourself from the lambda
	glfwSetWindowUserPointer(myWindowPtr->myGlfwWindow, this);

	glfwSetFramebufferSizeCallback(myWindowPtr->myGlfwWindow, [](GLFWwindow* Window, int X, int Y) -> void
	{
		Program* SelfPointer = static_cast<Program*>(glfwGetWindowUserPointer(Window));
		SelfPointer->OnWindowSizeChanged(Window, X, Y);
	});
}

void Program::OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y)
{
	myRendererPtr->SetViewportSize({ X, Y });
}

