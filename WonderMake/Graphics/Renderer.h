#pragma once
#include "Program/Window.h"
#include "Shader.h"

class Renderer : NonCopyable
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

private:
	Shader<EShaderType::Vertex> myVertexShader;
	Shader<EShaderType::Fragment> myFragmentShader;
	u32 myVAO;
	u32 myVBO;
	u32 myShaderProgram;

	const SColor ClearColor = SColor::CornflowerBlue;
};

