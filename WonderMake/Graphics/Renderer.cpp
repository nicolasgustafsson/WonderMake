#include "pch.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include <GLFW/glfw3.h>
#include "Program/GlfwFacade.h"

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
	: myRenderTarget({ {1600, 900}, true })
	, myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/Copy.frag")
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
	myCameraPtr->SetViewportSize({1600, 900});
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	myOpenGLInterface->SetViewportSize(WindowSize);
	myCameraPtr->SetViewportSize(WindowSize);
}

void Renderer::StartFrame()
{
	SystemPtr<GlfwFacade> glfw;

	glfw->SwapBuffers(myWindowPtr->myGlfwWindow);

	myEngineUniformBufferPtr->Update();

	//first pass
	myRenderTarget.BindAsTarget();

	myOpenGLInterface->SetClearColor(ClearColor);
	myOpenGLInterface->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::FinishFrame()
{
	myLineDrawer->Render();

	myOpenGLInterface->BindFramebuffer(GL_FRAMEBUFFER, 0);

	myOpenGLInterface->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	myOpenGLInterface->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	SystemPtr<GlfwFacade> glfw;

	myDebugWindowHasFocus = ImGui::IsWindowFocused();

	i32 windowX, windowY;
	glfw->GetWindowPos(myWindowPtr->myGlfwWindow, &windowX, &windowY);
	myCameraPtr->SetImguiWindowOffset(
		{ ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x - windowX
		, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y - windowY });

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	const SVector2i ViewportSize = { static_cast<i32>(ImGui::GetContentRegionAvail().x), static_cast<i32>(ImGui::GetContentRegionAvail().y) };
#pragma warning(disable: 4312 26493)
	ImGui::Image((ImTextureID)myRenderTarget.GetTexture(), ImVec2(static_cast<float>(ViewportSize.X), static_cast<float>(ViewportSize.Y)));
#pragma warning(default: 4312 26493)

	myCameraPtr->SetViewportSize(ViewportSize);

	ImGui::End();
}
