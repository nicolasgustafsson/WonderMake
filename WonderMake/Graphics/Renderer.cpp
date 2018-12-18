#include "stdafx.h"
#include "Renderer.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <chrono>
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

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

Renderer::Renderer()
	: mySpriteRenderObject(
		{ std::filesystem::current_path() / "Textures/tile.png" })
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	SLogMessage message;
	message.LogText = "Successfully setup renderer!";
	WmDispatchMessage(message);
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
