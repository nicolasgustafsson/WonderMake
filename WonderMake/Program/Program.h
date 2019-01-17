#pragma once
#include "Graphics/Renderer.h"
#include "Window.h"
#include "ImguiWrapper.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/ImGuiLogger.h"
#include "Imgui/DockSpace.h"
#include <System/SystemPtr.h>

class Program
{
public:
	Program();

	void Update();

private:

	void SetupCallbacks();
	void OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y);

	//-------KEEP THIS IN ORDER-------
	ConsoleLogger myLogger;
	ImGuiLogger myImGuiLogger;
	DockSpace myDockSpace;
	SystemPtr<Window> myWindowPtr;
	SystemPtr<Renderer> myRendererPtr;
	SystemPtr<ImguiWrapper> myImguiWrapperPtr;
	//-------KEEP THIS IN ORDER-------

	static constexpr SVector2<i32> WindowSize = {1600, 900};
};

