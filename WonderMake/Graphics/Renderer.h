#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"

class Renderer : NonCopyable
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

private:

	SpriteRenderObject mySpriteRenderObject;

	const SColor ClearColor = SColor::CornflowerBlue;
	float myRotation = 0.0f;
};

