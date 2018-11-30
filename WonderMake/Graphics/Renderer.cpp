#include "stdafx.h"
#include "Renderer.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Shader.h"

constexpr std::array<f32, 3 * 3> Vertices
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

Renderer::Renderer()
	:myVertexShader(std::filesystem::current_path() / "Shaders/Vertex/SpriteVertex.vert"),
	myFragmentShader(std::filesystem::current_path() / "Shaders/Fragment/SpriteFragment.frag")
{
	myShaderProgram = glCreateProgram();

	glGenBuffers(1, &myVBO);

	glAttachShader(myShaderProgram, myVertexShader.ShaderHandle);
	glAttachShader(myShaderProgram, myFragmentShader.ShaderHandle);
	glLinkProgram(myShaderProgram);

	i32 Success;
	char ErrorMessage[512];
	glGetProgramiv(myShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(myShaderProgram, 512, NULL, ErrorMessage);

		std::cout << "Error: Shader program linking failed\n" << ErrorMessage << std::endl;
	}

	glGenVertexArrays(1, &myVAO);

	glBindVertexArray(myVAO);

	constexpr u32 Size = static_cast<unsigned int>(Vertices.size() * sizeof(float));
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBufferData(GL_ARRAY_BUFFER, Size, &*Vertices.begin(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);
}

void Renderer::SetViewportSize(const SVector2<int> WindowSize)
{
	glViewport(0, 0, WindowSize.X, WindowSize.Y);
}

void Renderer::SwapFrame()
{
	glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(myShaderProgram);
	glBindVertexArray(myVAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}
