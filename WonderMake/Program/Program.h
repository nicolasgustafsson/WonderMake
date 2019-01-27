#pragma once
#include "Graphics/Renderer.h"
#include "Window.h"
#include "ImguiWrapper.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/FileLogger.h"
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
	FileLogger myFileLogger;
	ImGuiLogger myImGuiLogger;
	DockSpace myDockSpace;
	SystemPtr<Window> myWindowPtr;
	SystemPtr<Renderer> myRendererPtr;
	SystemPtr<EngineUniformBuffer> myEngineUniformBufferPtr;
	ImguiWrapper myImguiWrapper;
	//-------KEEP THIS IN ORDER-------

	SystemPtr<Camera> myCameraPtr;

	static constexpr SVector2<i32> WindowSize = {1600, 900};
};

