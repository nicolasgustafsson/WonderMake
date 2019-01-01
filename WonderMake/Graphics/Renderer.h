#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "LineRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"

class Renderer : NonCopyable
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

private:

	SpriteRenderObject mySpriteRenderObject;
	LineRenderObject myLine;
	RenderTarget myRenderTarget;
	ScreenPassRenderObject myCopyPass;

	const SColor ClearColor = SColor::CornflowerBlue;
	float myRotation = 0.0f;
};

