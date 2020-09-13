#include "pch.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include <GLFW/glfw3.h>
#include "Program/GlfwFacade.h"

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

Renderer::Renderer(Dependencies&& aDependencies) noexcept
	: Super(std::move(aDependencies))
	, Debugged("Renderer")
	, myRenderTarget(SRenderTargetSettings{ {1600, 900}, false })
	, myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/Copy.frag")
{
	Get<OpenGLFacade>().Enable(GL_DEBUG_OUTPUT);
	Get<OpenGLFacade>().Enable(GL_BLEND);

	Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Get<OpenGLFacade>().SetBlendFunction(GL_SRC_ALPHA, GL_ONE); additive

	Get<OpenGLFacade>().SetDebugMessageCallback(MessageCallback);
	Get<Camera>().SetViewportSize({ 1600, 900 });
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	Get<OpenGLFacade>().SetViewportSize(WindowSize);
	Get<Camera>().SetViewportSize(WindowSize);
}

void Renderer::StartFrame()
{
	auto& glfw = Get<GlfwFacade>();

	glfw.SwapBuffers(Get<Window>().myGlfwWindow);

	Get<EngineUniformBuffer>().Update();

	//first pass
	myRenderTarget.BindAsTarget();

	Get<OpenGLFacade>().SetClearColor(ClearColor);
	Get<OpenGLFacade>().Clear(GL_COLOR_BUFFER_BIT);
}

void Renderer::FinishFrame()
{
	Get<DebugLineDrawer>().Render();

	Get<OpenGLFacade>().BindFramebuffer(GL_FRAMEBUFFER, 0);

	Get<OpenGLFacade>().SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Get<OpenGLFacade>().Clear(GL_COLOR_BUFFER_BIT);

	if constexpr (!Constants::IsDebugging)
	{
		//second pass - copy directly to backbuffer if we are not debugging
		myRenderTarget.BindAsTexture();

		myCopyPass.Render();
	}
}

void Renderer::Debug()
{
	//if we are debugging, render the game window as an imgui image

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	
	ImGui::Begin("Game Window");
	auto& glfw = Get<GlfwFacade>();

	myDebugWindowHasFocus = ImGui::IsWindowFocused();

	i32 windowX, windowY;
	glfw.GetWindowPos(Get<Window>().myGlfwWindow, &windowX, &windowY);
	Get<Camera>().SetImguiWindowOffset(
		{ ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x - windowX
		, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y - windowY });

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	const SVector2i ViewportSize = { static_cast<i32>(ImGui::GetContentRegionAvail().x), static_cast<i32>(ImGui::GetContentRegionAvail().y) };
#pragma warning(disable: 4312 26493)
	ImGui::Image((ImTextureID)myRenderTarget.GetTexture(), ImVec2(static_cast<float>(ViewportSize.X), static_cast<float>(ViewportSize.Y)));
#pragma warning(default: 4312 26493)

	Get<Camera>().SetViewportSize(ViewportSize);

	ImGui::End();
}
