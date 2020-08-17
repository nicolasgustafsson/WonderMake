#include "pch.h"
#include "Program.h"
#include <GLFW/glfw3.h>
#include <functional>
#include "Graphics/Shader.h"
#include <iostream>
#include "../Threads/DataThreads.h"
#include "Camera/Camera.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Imgui/ImguiInclude.h"
#include "Program/GlfwFacade.h"

Program::Program()
{
	SetupCallbacks();
}

void Program::Update()
{
	FinishPreviousFrame();

	myInputSystem->Update();
	myAudioManagerPtr->Update();

	if constexpr (Constants::EnableAssetHotReload)
		myFileWatcher->UpdateFileChanges();

	StartNewFrame();
}

void Program::StartNewFrame()
{
	myEngineUniformBufferPtr->GetBuffer().Time = myTimeKeeperPtr->GetGameTime();

	myWindowPtr->Update();

	myCameraManagerPtr->Update();

	myRendererPtr->StartFrame();
}

void Program::FinishPreviousFrame()
{
	if constexpr (!Constants::IsDebugging)
		myRendererPtr->FinishFrame();
}

void Program::SetupCallbacks()
{
	SystemPtr<GlfwFacade> glfw;
	//sets the user pointer so we can access ourself from the lambda
	glfw->SetWindowUserPointer(myWindowPtr->myGlfwWindow, this);

	glfw->SetFramebufferSizeCallback(myWindowPtr->myGlfwWindow, [](GLFWwindow* Window, int X, int Y) -> void
	{
			SystemPtr<GlfwFacade> glfw;
			Program* SelfPointer = static_cast<Program*>(glfw->GetWindowUserPointer(Window));
		SelfPointer->OnWindowSizeChanged(Window, X, Y);
	});
}

void Program::OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y)
{
	myRendererPtr->SetViewportSize({ X, Y });
}

