#include "pch.h"
#include "ProgramSystem.h"

#include "Audio/AudioManager.h"
#include "Camera/CameraManager.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Program/GlfwFacade.h"
#include "Program/Window.h"
#include "WonderMakeEngine/SystemPtr.h"
#include "Utilities/TimeKeeper.h"

REGISTER_SYSTEM(ProgramSystem);

using WindowSizeCallback = UniqueFunction<void(GLFWwindow*, int, int)>;

ProgramSystem::ProgramSystem()
{
	myThisPointer = std::make_shared<ProgramSystem*>(this);

	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });

	SetupCallbacks();
}

ProgramSystem::~ProgramSystem()
{
	auto&& glfw = Get<GlfwFacade>();
	auto glfwWindow = Get<Window>().myGlfwWindow;

	auto meta = glfw.GetWindowUserPointer(glfwWindow);

	if (meta)
	{
		delete static_cast<WindowSizeCallback*>(meta);
	}

	glfw.SetWindowUserPointer(glfwWindow, nullptr);
	glfw.SetFramebufferSizeCallback(glfwWindow, nullptr);
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

void ProgramSystem::SetupCallbacks()
{
	auto&& glfw = Get<GlfwFacade>();
	auto glfwWindow = Get<Window>().myGlfwWindow;

	auto meta = glfw.GetWindowUserPointer(glfwWindow);

	if (meta)
	{
		delete static_cast<WindowSizeCallback*>(meta);
	}

	auto closure = std::make_unique<WindowSizeCallback>([weakThis = std::weak_ptr(myThisPointer)](GLFWwindow* aWindow, int aWidth, int aHeight)
	{
		auto ptr = weakThis.lock();

		if (ptr)
		{
			(*ptr)->OnWindowSizeChanged(aWindow, aWidth, aHeight);
		}
	});

	glfw.SetWindowUserPointer(glfwWindow, closure.get());
	glfw.SetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* aWindow, int aWidth, int aHeight) -> void
		{
			SystemPtr<GlfwFacade> glfw;

			static_cast<WindowSizeCallback*>(glfw->GetWindowUserPointer(aWindow))->operator()(aWindow, aWidth, aHeight);
		});

	closure.release();
}

void ProgramSystem::OnWindowSizeChanged(GLFWwindow* /*aWindow*/, i32 aWidth, i32 aHeight)
{
	if constexpr (!Constants::IsDebugging)
	{
		Get<CameraManager>().SetViewportSize({ aWidth, aHeight });
	}
}

