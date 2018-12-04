#include "stdafx.h"
#include "Renderer.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <chrono>

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

Renderer::Renderer()
	:myShaderProgram(
		std::filesystem::current_path() / "Shaders/Vertex/SpriteVertex.vert", 
		std::filesystem::current_path() / "Shaders/Fragment/SpriteFragment.frag"
		, std::filesystem::current_path() / "Shaders/Geometry/SpriteGeometry.geom"
	), 
	myQuadVertexBuffer({}),
	myTexture(std::filesystem::current_path() / "Textures/tile.png")
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
	
	myQuadVertexBuffer.Bind();
	myShaderProgram.Activate();
	myTexture.Bind();

	glBindVertexArray(myVAO);

	glDrawArrays(GL_POINTS, 0, 1);
}
