#include "pch.h"

#include "Window.h"

#include "Program/GlfwFacade.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/BufferExecutor.h"

#include "GLFW/glfw3.h"

#include "Json/json.hpp"

#include <fstream>

REGISTER_SYSTEM(Window);

using WindowSizeCallback = std::function<void(GLFWwindow*, int, int)>;

inline constexpr SVector2i locDefaultWindowSize = { 1280, 720 };

Window::~Window()
{
	auto& glfw = Get<GlfwFacade>();

	auto meta = glfw.GetWindowUserPointer(myGlfwWindow);

	if (meta)
		delete static_cast<WindowSizeCallback*>(meta);

	glfw.SetWindowUserPointer(myGlfwWindow, nullptr);
	glfw.SetFramebufferSizeCallback(myGlfwWindow, nullptr);
}

void Window::Initialize()
{
	auto&& configSys = Get<ConfigurationSystem>();
	
	auto windowName = configSys.Get<std::string>(ConfigurationEngine::WindowTitle, "WonderMake");

	auto& glfw = Get<GlfwFacade>();
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfw.SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfw.SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mySize =
	{
		configSys.Get<i32>(ConfigurationEngine::WindowWidth,	locDefaultWindowSize.X),
		configSys.Get<i32>(ConfigurationEngine::WindowHeight,	locDefaultWindowSize.Y)
	};

	myGlfwWindow = glfw.CreateGlfwWindow(mySize.X, mySize.Y, windowName.c_str(), NULL, NULL);
	if (!myGlfwWindow)
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Failed to create GLFW window.");
		glfw.Terminate();
		return;
	}

	glfw.MakeContextCurrent(myGlfwWindow);

	if (!glfw.InitializeGlad())
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Failed to initialize GLAD.");
	}

	UpdatePosition();
	myHasFocus = glfw.GetWindowAttrib(myGlfwWindow, GLFW_FOCUSED) != 0;

	auto closure = std::make_unique<WindowSizeCallback>(Bind(&Window::OnWindowSizeChanged, weak_from_this()))
		.release();

	glfw.SetWindowUserPointer(myGlfwWindow, closure);
	glfw.SetFramebufferSizeCallback(myGlfwWindow, [](GLFWwindow* aWindow, int aWidth, int aHeight) -> void
		{
			GlfwFacade& glfw = GetSystem<GlfwFacade>();

			const auto& callback = *static_cast<WindowSizeCallback*>(glfw.GetWindowUserPointer(aWindow));

			callback(aWindow, aWidth, aHeight);
		});

	auto onTitleChange = [this](auto&&, std::string aTitle)
	{
		auto& glfw = Get<GlfwFacade>();

		WmLogInfo(TagWonderMake << TagWmOpenGL << "Window title changed to " << aTitle << '.');

		glfw.SetWindowTitle(myGlfwWindow, aTitle.c_str());
	};
	auto onSizeChange = [this](auto&&...)
	{
		auto& configSys = Get<ConfigurationSystem>();
		auto& glfw = Get<GlfwFacade>();

		SVector2i newSize =
		{
			configSys.Get<i32>(ConfigurationEngine::WindowWidth,	locDefaultWindowSize.X),
			configSys.Get<i32>(ConfigurationEngine::WindowHeight,	locDefaultWindowSize.Y)
		};

		if (newSize == mySize)
			return;

		mySize = newSize;

		WmLogInfo(TagWonderMake << TagWmOpenGL << "Window size changed to " << mySize.X << ',' << mySize.Y << '.');

		glfw.SetWindowSize(myGlfwWindow, mySize.X, mySize.Y);
	};

	mySubscriberWindowTitle	= configSys.OnOverrideChanged<std::string>(ConfigurationEngine::WindowTitle,	GetExecutor(), onTitleChange);
	mySubscriberSizeWidth	= configSys.OnOverrideChanged<i32>(ConfigurationEngine::WindowWidth,			GetExecutor(), onSizeChange);
	mySubscriberSizeHeight	= configSys.OnOverrideChanged<i32>(ConfigurationEngine::WindowHeight,			GetExecutor(), onSizeChange);
}

void Window::Update()
{
	auto& glfw = Get<GlfwFacade>();

	glfw.PollEvents();

	if (glfw.ShouldWindowClose(myGlfwWindow))
		quick_exit(0);

	myHasFocus = glfw.GetWindowAttrib(myGlfwWindow, GLFW_FOCUSED) != 0;
	UpdatePosition();
}

void Window::OnWindowSizeChanged(GLFWwindow* /*aWindow*/, i32 aWidth, i32 aHeight)
{
	mySize = { aWidth, aHeight };

	BufferExecutor executor;

	for (auto& onResize : myResizeList)
		onResize.Trigger(executor, SVector2i(aWidth, aHeight));

	executor.ExecuteAll();
}

void Window::UpdatePosition()
{
	auto& glfw = Get<GlfwFacade>();
	int x = 0;
	int y = 0;

	glfw.GetWindowPos(myGlfwWindow, &x, &y);

	myPosition.X = static_cast<i32>(x);
	myPosition.Y = static_cast<i32>(y);
}
