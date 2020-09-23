#include "pch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Json/json.hpp"
#include <fstream>
#include "Program/GlfwFacade.h"
#include "CameraManager.h"

Window::Window()
{
	std::ifstream windowSettingsFile("windowSettings.json");
	json windowSettings;

	windowSettingsFile >> windowSettings;

	SystemPtr<GlfwFacade> glfw;
	glfw->Init();
	glfw->SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfw->SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfw->SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	SVector2i windowSize = { windowSettings["X"].get<i32>(), windowSettings["Y"].get<i32>() };
	myGlfwWindow = glfw->CreateGlfwWindow(windowSize.X, windowSize.Y, "WonderMake", NULL, NULL);
	if (!myGlfwWindow)
	{
		WmLog(TagError, TagOpenGL, "Failed to create GLFW window!");
		glfw->Terminate();
		return;
	}

	glfw->MakeContextCurrent(myGlfwWindow);

	if (!glfw->InitializeGlad())
	{
		WmLog(TagError, TagOpenGL, "Failed to initialize GLAD");
	}

	if constexpr (!Constants::IsDebugging)
		SystemPtr<CameraManager>()->SetViewportSize(windowSize);
}

Window::~Window()
{
}

void Window::Update()
{
	SystemPtr<GlfwFacade> glfw;

	if (glfw->GetKey(myGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfw->SetWindowShouldClose(myGlfwWindow, true);

	glfw->PollEvents();

	if (glfw->ShouldWindowClose(myGlfwWindow))
	{
		WmDispatchTask([]() {quick_exit(0);}, ERoutineId::Logic);
	}
}

bool Window::IsValid() const
{
	return myGlfwWindow != nullptr;
}