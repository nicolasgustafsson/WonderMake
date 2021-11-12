#include "pch.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include <GLFW/glfw3.h>
#include "Program/GlfwFacade.h"
#include "Graphics/RenderCommandProcessor.h"
#include <any>
#include "Graphics/RenderTarget.h"

REGISTER_SYSTEM(Renderer);

void GLAPIENTRY
MessageCallback([[maybe_unused]] GLenum source,
	GLenum type,
	[[maybe_unused]]GLuint id,
	GLenum severity,
	[[maybe_unused]]GLsizei length,
	const GLchar* message,
	[[maybe_unused]]const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	auto severityTag = severity == GL_DEBUG_SEVERITY_HIGH ? TagError : (severity == GL_DEBUG_SEVERITY_MEDIUM) ? TagWarning : "";

	WmLog(severityTag, TagOpenGL, " type = ", type, " severity: ", severity, "\n", message);
}

Renderer::Renderer() noexcept
	: myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/BackbufferCopy.frag")
	, Debugged("Renderer")
{
	Get<OpenGLFacade>().Enable(GL_DEBUG_OUTPUT);
	Get<OpenGLFacade>().Enable(GL_BLEND);
	Get<OpenGLFacade>().Enable(GL_MULTISAMPLE);

	Get<OpenGLFacade>().Enable(GL_DEPTH_TEST);

	glDepthFunc(GL_GEQUAL);
	glClearDepth(-1000);

	Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Get<OpenGLFacade>().SetDebugMessageCallback(MessageCallback);
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	Get<OpenGLFacade>().SetViewportSize(WindowSize);
	//myCameraManagerPtr->GetMainCamera().SetViewportSize(WindowSize);
}

void Renderer::StartFrame()
{
	if constexpr (Constants::IsDebugging)
	{
		Get<GlfwFacade>().SwapBuffers(Get<Window>().myGlfwWindow);
	}
}

void Renderer::FinishFrame()
{
	WmDispatchMessage(RenderPassMessage{});

	Get<CameraManager>().FinishFrame();

	Get<DebugLineDrawer>().Update();

	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);

	Get<OpenGLFacade>().SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Get<OpenGLFacade>().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if constexpr (!Constants::IsDebugging)
	{
		//second pass - copy directly to backbuffer if we are not debugging
		//myCameraManagerPtr->GetMainCamera().BindAsTexture();

		myCopyPass.RenderImmediate();

		Get<GlfwFacade>().SwapBuffers(Get<Window>().myGlfwWindow);
	}

	auto&& renderCommandProcessors = Get<UniverseSystemCollection<RenderCommandProcessor>>().GetAllSystems();

	for (auto& renderCommandProcessor : renderCommandProcessors)
	{
		renderCommandProcessor->Clear();
	}
}

void Renderer::Debug()
{
	Get<CameraManager>().FinishDebugFrame();
}