#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "CameraManager.h"
#include "System/SystemPtr.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"
#include "Debugging/DebugLineDrawer.h"
#include "OpenGLFacade.h"


class Renderer 
	: public System
	, public Debugged
{
public:
	Renderer() noexcept;

	void SetViewportSize(const SVector2<int> WindowSize);

	void StartFrame();

	void FinishFrame();

	bool DebugWindowHasFocus() const noexcept { return myDebugWindowHasFocus; };

private:
	virtual void Debug() override;

	ScreenPassRenderObject myCopyPass;
	SystemPtr<CameraManager> myCameraManagerPtr;
	SystemPtr<EngineUniformBuffer> myEngineUniformBufferPtr;
	SystemPtr<Window> myWindowPtr;
	SystemPtr<DebugLineDrawer> myLineDrawer;
	SystemPtr<OpenGLFacade> myOpenGLInterface;

	bool myDebugWindowHasFocus = true;
};

