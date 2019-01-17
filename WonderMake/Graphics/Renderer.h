#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "LineRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include <System/System.h>

class Renderer : public System
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void SwapFrame();

	void Debug();

private:

	SpriteRenderObject mySpriteRenderObject;
	LineRenderObject myLine;
	RenderTarget myRenderTarget;
	ScreenPassRenderObject myCopyPass;

	const SColor ClearColor = SColor::CornflowerBlue;
	float myRotation = 0.0f;
};

