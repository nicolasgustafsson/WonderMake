#pragma once
#include "RenderObject.h"

class Renderer : NonCopyable
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

private:
	u32 myVAO;
	u32 myEBO;

	RenderObject<> myRenderObject;

	const SColor ClearColor = SColor::CornflowerBlue;
};

