#pragma once

#include "System/System.h"

#include <memory>

class AudioManager;
class CameraManager;
class EngineUniformBuffer;
class GlfwFacade;
class InputSystem;
class ScheduleSystem;
class TimeKeeper;
class Renderer;
class Window;

class ProgramSystem
	: public System<
		Policy::Set<
			PAdd<AudioManager, PWrite>,
			PAdd<CameraManager, PWrite>,
			PAdd<EngineUniformBuffer, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<InputSystem, PWrite>,
			PAdd<ScheduleSystem, PWrite>,
			PAdd<TimeKeeper, PRead>,
			PAdd<Renderer, PWrite>,
			PAdd<Window, PWrite>>>
{
public:
	ProgramSystem();
	~ProgramSystem();

	void Tick();

	void StartNewFrame();

	void FinishPreviousFrame();

private:
	void SetupCallbacks();
	void OnWindowSizeChanged(GLFWwindow* aWindow, i32 aWidth, i32 aHeight);

	std::shared_ptr<ProgramSystem*> myThisPointer;

	static constexpr SVector2<i32> WindowSize = { 1600, 900 };
};

