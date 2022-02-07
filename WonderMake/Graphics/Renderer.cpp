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

#include "WonderMakeBase/Logger.h"

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

	const auto logSeverity = severity == GL_DEBUG_SEVERITY_HIGH ? ELogSeverity::Error : (severity == GL_DEBUG_SEVERITY_MEDIUM) ? ELogSeverity::Warning : ELogSeverity::Info;

	WM_LOG(logSeverity, ELogLevel::Normal, TagOpenGL, " type: ", type, ", severity: ", severity, ", message: {", message, "}.");
}

Renderer::Renderer() noexcept
	: Debugged("Renderer")
{
	Get<OpenGLFacade>().Enable(GL_DEBUG_OUTPUT);
	Get<OpenGLFacade>().Enable(GL_BLEND);

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
	Get<CameraManager>().FinishFrame();

	Get<DebugLineDrawer>().Update();

	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);

	Get<OpenGLFacade>().SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Get<OpenGLFacade>().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if constexpr (!Constants::IsDebugging)
	{
		//second pass - copy directly to backbuffer if we are not debugging
		//myCameraManagerPtr->GetMainCamera().BindAsTexture();

		if (!myCopyPass)
			myCopyPass.emplace(Get<ResourceSystem<Shader<EShaderType::Vertex>>>(), Get<ResourceSystem<Shader<EShaderType::Fragment>>>(), Get<ResourceSystem<Shader<EShaderType::Geometry>>>(), std::filesystem::current_path() / "Shaders/Fragment/BackbufferCopy.frag");

		myCopyPass->RenderImmediate();

		Get<GlfwFacade>().SwapBuffers(Get<Window>().myGlfwWindow);
	}

	Get<RenderCommandProcessor>().Clear();
}

void Renderer::Debug()
{
	Get<CameraManager>().FinishDebugFrame();
}
