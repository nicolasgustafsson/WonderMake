#include "pch.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include <GLFW/glfw3.h>
#include "Program/GlfwFacade.h"
#include "Graphics/RenderCommandProcessor.h"

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
	: myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/Copy.frag")
	, Debugged("Renderer")
{
	myOpenGLInterface->Enable(GL_DEBUG_OUTPUT);
	myOpenGLInterface->Enable(GL_BLEND);

	myOpenGLInterface->Enable(GL_DEPTH_TEST);

	glDepthFunc(GL_GEQUAL);
	glClearDepth(-1000);

	myOpenGLInterface->SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//myOpenGLInterface->SetBlendFunction(GL_SRC_ALPHA, GL_ONE); //additive

	myOpenGLInterface->SetDebugMessageCallback(MessageCallback);
	myCameraManagerPtr->GetMainCamera().SetViewportSize({1600, 900});
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	myOpenGLInterface->SetViewportSize(WindowSize);
	myCameraManagerPtr->GetMainCamera().SetViewportSize(WindowSize);
}

void Renderer::StartFrame()
{
	SystemPtr<GlfwFacade> glfw;

	glfw->SwapBuffers(myWindowPtr->myGlfwWindow);
}

void Renderer::FinishFrame()
{
	myLineDrawer->Render();

	myCameraManagerPtr->FinishFrame();

	myOpenGLInterface->BindFramebuffer(GL_FRAMEBUFFER, 0);

	myOpenGLInterface->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	myOpenGLInterface->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SystemPtr<RenderCommandProcessor>()->ClearQueue();

	if constexpr (!Constants::IsDebugging)
	{
		//second pass - copy directly to backbuffer if we are not debugging
		//myRenderTarget.BindAsTexture();

		//myCopyPass.Render();
	}
}

void Renderer::Debug()
{
	SystemPtr<CameraManager>()->FinishDebugFrame();
}
