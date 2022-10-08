#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/EventSubscriber.h"

#include <memory>

class AudioManager;
class CameraManager;
class EngineUniformBuffer;
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
			PAdd<InputSystem, PWrite>,
			PAdd<ScheduleSystem, PWrite>,
			PAdd<TimeKeeper, PRead>,
			PAdd<Renderer, PWrite>,
			PAdd<Window, PWrite>>,
		STrait::Set<
			STGui>>
	, public std::enable_shared_from_this<ProgramSystem>
{
public:
	ProgramSystem();

	void Initialize() override;

	void Tick();

	void StartNewFrame();

	void FinishPreviousFrame();

private:
	void OnWindowSizeChanged(SVector2i aSize);

	EventSubscriber mySubscriberWindowResize;
};

