#include "stdafx.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"
#include <GLFW/glfw3.h>

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

Renderer::Renderer()
	: myRenderTarget({ 1600, 900 })
	, myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/Copy.frag")
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	myCameraPtr->SetViewportSize({1600, 900});
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	glViewport(0, 0, WindowSize.X, WindowSize.Y);
	myCameraPtr->SetViewportSize(WindowSize);
}

void Renderer::StartFrame()
{
	glfwSwapBuffers(myWindowPtr->myGlfwWindow);

	myEngineUniformBufferPtr->Update();

	//first pass
	myRenderTarget.BindAsTarget();

	glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::FinishFrame()
{
	myLineDrawer->Render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

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

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	SVector2i ViewportSize = { static_cast<i32>(ImGui::GetContentRegionAvail().x), static_cast<i32>(ImGui::GetContentRegionAvail().y) };
	ImGui::Image((void *)myRenderTarget.GetTexture(), ImVec2(ViewportSize.X, ViewportSize.Y));

	myCameraPtr->SetViewportSize(ViewportSize);

	ImGui::End();
}
