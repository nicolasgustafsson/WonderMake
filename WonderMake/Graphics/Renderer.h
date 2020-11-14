#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Camera/Camera.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"
#include "Debugging/DebugLineDrawer.h"
#include "OpenGLFacade.h"

class GlfwFacade;

class Renderer
	: public System<
		Policy::Set<
			PAdd<Camera, PWrite>,
			PAdd<EngineUniformBuffer, PWrite>,
			PAdd<Window, PWrite>,
			PAdd<DebugLineDrawer, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<OpenGLFacade, PWrite>>>
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

	RenderTarget myRenderTarget;
	ScreenPassRenderObject myCopyPass;

	bool myDebugWindowHasFocus = false;

	const SColor ClearColor = SColor::Grey;
};
