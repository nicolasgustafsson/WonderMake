#include "stdafx.h"
#include "Renderer.h"

#include "VertexAttributes.h"
#include "EngineUniformBuffer.h"
#include "Camera/Camera.h"

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
	:	mySpriteRenderObject({ std::filesystem::current_path() / "Textures/tile.png" })
	,	myRenderTarget({ 1600, 900 })
	,	myCopyPass(std::filesystem::current_path() / "Shaders/Fragment/Copy.frag")
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

void Renderer::SwapFrame()
{
	myEngineUniformBufferPtr->Update();

	//first pass
	myRenderTarget.BindAsTarget();

	glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);

	myRotation += 0.003f;
	mySpriteRenderObject.SetAttribute<EVertexAttribute::Rotation>(0, myRotation);

	mySpriteRenderObject.Render();

	SVector2f topLeft = { -100.f, -100.f };
	SVector2f topRight = { 100.f, -100.f };
	SVector2f bottomLeft = { -100.f, 100.f };
	SVector2f bottomRight = { 100.f, 100.f };

	myLine.SetAttribute<EVertexAttribute::Position>(0, topLeft);
	myLine.SetAttribute<EVertexAttribute::Position>(1, topRight);

	myLine.SetAttribute<EVertexAttribute::Position>(2, topRight);
	myLine.SetAttribute<EVertexAttribute::Position>(3, bottomRight);

	myLine.SetAttribute<EVertexAttribute::Position>(4, bottomRight);
	myLine.SetAttribute<EVertexAttribute::Position>(5, bottomLeft);

	myLine.SetAttribute<EVertexAttribute::Position>(6, bottomLeft);
	myLine.SetAttribute<EVertexAttribute::Position>(7, topLeft);

	myLine.SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 0.0f, 0.0f, 1.0f});
	myLine.SetAttribute<EVertexAttribute::Color>(1, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(2, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(3, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(4, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(5, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(6, { 1.0f, 0.0f, 0.0f, 1.0f });
	myLine.SetAttribute<EVertexAttribute::Color>(7, { 1.0f, 0.0f, 0.0f, 1.0f });

	myLine.Render();

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
	if constexpr (Constants::IsDebugging)
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
}
