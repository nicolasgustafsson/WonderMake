#pragma once
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Texture.h"

class Renderer : NonCopyable
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

private:
	ShaderProgram myShaderProgram;
	u32 myVAO;
	u32 myEBO;

	VertexBuffer<SVector3<f32>> myQuadVertexBuffer;
	Texture myTexture;

	const SColor ClearColor = SColor::CornflowerBlue;
};

