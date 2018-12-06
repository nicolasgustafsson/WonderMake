#include "stdafx.h"
#include "Renderer.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <chrono>

void GLAPIENTRY
MessageCallback([[maybe_unused]] GLenum source,
	GLenum type,
	[[maybe_unused]]GLuint id,
	GLenum severity,
	[[maybe_unused]]GLsizei length,
	const GLchar* message,
	[[maybe_unused]]const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

Renderer::Renderer()
	: myRenderObject({ 
	std::filesystem::current_path() / "Shaders/Vertex/SpriteVertex.vert",
	std::filesystem::current_path() / "Shaders/Geometry/SpriteGeometry.geom",
	std::filesystem::current_path() / "Shaders/Fragment/SpriteFragment.frag",
	std::filesystem::current_path() / "Textures/tile.png",
	std::array<SGenericVertex, 1> {0.0f}})
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glGenVertexArrays(1, &myVAO);
	glBindVertexArray(myVAO);
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	glViewport(0, 0, WindowSize.X, WindowSize.Y);
}

void Renderer::SwapFrame()
{
	glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);
	
	myRenderObject.Render();
}
