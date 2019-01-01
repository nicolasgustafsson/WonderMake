#include "stdafx.h"
#include "Renderer.h"

#include "VertexAttributes.h"

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
	: mySpriteRenderObject(
		{ std::filesystem::current_path() / "Textures/tile.png" })
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	glViewport(0, 0, WindowSize.X, WindowSize.Y);
}

void Renderer::SwapFrame()
{
	glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);

	myRotation += 0.003f;
	mySpriteRenderObject.SetAttribute<EVertexAttribute::Rotation>(0, myRotation);

	mySpriteRenderObject.Render();

	myLine.SetAttribute<EVertexAttribute::Position>(0, { 0.1f, 0.2f });
	myLine.SetAttribute<EVertexAttribute::Position>(1, { 0.4f, 0.2f });

	myLine.SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 0.0f, 0.0f, 1.0f});
	myLine.SetAttribute<EVertexAttribute::Color>(1, { 1.0f, 1.0f, 0.0f, 1.0f});

	myLine.Render();
}
