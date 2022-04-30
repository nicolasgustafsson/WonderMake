#include "pch.h"
#include "Program.h"
#include <GLFW/glfw3.h>
#include <functional>
#include "Graphics/Shader.h"
#include <iostream>
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

	SystemPtr<InputSystem>()->Update();
	SystemPtr<AudioManager>()->Update();

	if constexpr (Constants::EnableAssetHotReload)
	{
		SystemPtr<FileWatcher>()->UpdateFileChanges();
	}

	StartNewFrame();
}

void Program::StartNewFrame()
{
	SystemPtr<EngineUniformBuffer>()->GetBuffer().Time = SystemPtr<Wm::TimeKeeper>()->GetGameTime();

	SystemPtr<EngineUniformBuffer>()->Update();

	SystemPtr<Window>()->Update();

	SystemPtr<Renderer>()->StartFrame();
}

void Program::FinishPreviousFrame()
{
	if constexpr (!Constants::IsDebugging)
		SystemPtr<Renderer>()->FinishFrame();
}

void Program::SetupCallbacks()
{
	SystemPtr<GlfwFacade> glfw;
	//sets the user pointer so we can access ourself from the lambda
	glfw->SetWindowUserPointer(SystemPtr<Window>()->myGlfwWindow, this);

	glfw->SetFramebufferSizeCallback(SystemPtr<Window>()->myGlfwWindow, [](GLFWwindow* Window, int X, int Y) -> void
	{
			SystemPtr<GlfwFacade> glfw;
			Program* SelfPointer = static_cast<Program*>(glfw->GetWindowUserPointer(Window));
		SelfPointer->OnWindowSizeChanged(Window, X, Y);
	});
}

void Program::OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y)
{
	if constexpr (!Constants::IsDebugging)
	{
		SystemPtr<CameraManager>()->SetViewportSize({X, Y});
	}
}

