#pragma once
#include "Graphics/Renderer.h"
#include "Window.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/FileLogger.h"
#include "Logging/ImGuiLogger.h"
#include "Imgui/DockSpace.h"
#include "System/SystemPtr.h"
#include "Utilities/TimeKeeper.h"
#include "Input/InputSystem.h"
#include "Debugging/DebugSettingsSystem.h"
#include "Audio/AudioManager.h"
#include "FileSystem/FileWatcher.h"
#include "Camera/CameraManager.h"
#include "Program/ImguiWrapper.h"
#include "EngineTools/EngineTools.h"


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
	DockSpace myDockSpace;
	ConsoleLogger myLogger;
	FileLogger myFileLogger;
	ImGuiLogger myImGuiLogger;

	static constexpr SVector2<i32> WindowSize = {1600, 900};
};

