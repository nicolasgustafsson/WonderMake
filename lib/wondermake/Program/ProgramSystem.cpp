#include "pch.h"
#include "ProgramSystem.h"

#include "Audio/AudioManager.h"
#include "Camera/CameraManager.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Program/Window.h"
#include "Utilities/TimeKeeper.h"

#include "wondermake-utility/Bindable.h"

REGISTER_SYSTEM(ProgramSystem);

ProgramSystem::ProgramSystem()
{
	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });
}

void ProgramSystem::Initialize()
{
	auto& window = Get<Window>();

	if constexpr (!Constants::IsDebugging)
		mySubscriberWindowResize = window.OnResize(GetExecutor(), Bind(&ProgramSystem::OnWindowSizeChanged, weak_from_this()));
}

void ProgramSystem::Tick()
{
	FinishPreviousFrame();

	Get<InputSystem>().Update();
	Get<AudioManager>().Update();

	StartNewFrame();
}

void ProgramSystem::StartNewFrame()
{
	Get<EngineUniformBuffer>().GetBuffer().Time = Get<TimeKeeper>().GetGameTime();

	Get<EngineUniformBuffer>().Update();

	Get<Window>().Update();

	Get<Renderer>().StartFrame();
}

void ProgramSystem::FinishPreviousFrame()
{
	if constexpr (!Constants::IsDebugging)
		Get<Renderer>().FinishFrame();
}

void ProgramSystem::OnWindowSizeChanged(SVector2i aSize)
{
	Get<CameraManager>().SetViewportSize(aSize);
}

