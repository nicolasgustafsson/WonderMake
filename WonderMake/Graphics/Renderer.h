#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "LineRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Camera/Camera.h"
#include "System/SystemPtr.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"

class Renderer 
	: public System
{
public:
	Renderer();

	void SetViewportSize(const SVector2<int> WindowSize);

	void StartFrame();

	void FinishFrame();


private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber mySubscriber;
	SpriteRenderObject mySpriteRenderObject;
	LineRenderObject myLine;
	RenderTarget myRenderTarget;
	ScreenPassRenderObject myCopyPass;
	SystemPtr<Camera> myCameraPtr;
	SystemPtr<EngineUniformBuffer> myEngineUniformBufferPtr;
	SystemPtr<Window> myWindowPtr;

	const SColor ClearColor = SColor::CornflowerBlue;
	float myRotation = 0.0f;
};

