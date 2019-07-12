#include "stdafx.h"
#include "Program.h"
#include <GLFW/glfw3.h>
#include <functional>
#include "Graphics/Shader.h"
#include <iostream>
#include "../Threads/DataThreads.h"
#include "Camera/Camera.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Imgui/ImguiInclude.h"

Program::Program()
{
	SetupCallbacks();
}

void Program::Update()
{
	FinishPreviousFrame();

	myInputSystem->Update();

	StartNewFrame();
}

void Program::StartNewFrame()
{
	myEngineUniformBufferPtr->GetBuffer().Time = myTimeKeeperPtr->GetGameTime();

	myWindowPtr->Update();

	myCameraPtr->Update();

	myRendererPtr->StartFrame();
}

void Program::FinishPreviousFrame()
{
	if constexpr (!Constants::IsDebugging)
		myRendererPtr->FinishFrame();
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

