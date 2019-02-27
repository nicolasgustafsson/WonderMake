#pragma once
#include "Graphics/Renderer.h"
#include "Window.h"
#include "ImguiWrapper.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/FileLogger.h"
#include "Logging/ImGuiLogger.h"
#include "Imgui/DockSpace.h"
#include "System/SystemPtr.h"
#include "TimeKeeper.h"
#include "Input/InputSystem.h"

class Program
{
public:
	Program();

	void Update();

	void StartNewFrame();

	void FinishPreviousFrame();

private:

	void SetupCallbacks();
	void OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y);

	//-------KEEP THIS IN ORDER-------
	DockSpace myDockSpace;
	ConsoleLogger myLogger;
	FileLogger myFileLogger;
	ImGuiLogger myImGuiLogger;
	SystemPtr<Window> myWindowPtr;
	SystemPtr<Renderer> myRendererPtr;
	SystemPtr<EngineUniformBuffer> myEngineUniformBufferPtr;
	//-------KEEP THIS IN ORDER-------

	SystemPtr<Camera> myCameraPtr;
	SystemPtr<TimeKeeper> myTimeKeeperPtr;
	SystemPtr<InputSystem> myInputSystem;

	static constexpr SVector2<i32> WindowSize = {1600, 900};
};

