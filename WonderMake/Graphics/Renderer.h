#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Camera/Camera.h"
#include "System/SystemPtr.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"
#include "Debugging/DebugLineDrawer.h"
#include "OpenGLFacade.h"

class GlfwFacade;

class Renderer
	: public System<
		Policy::Set<
			Policy::Add<Camera, Policy::EPermission::Write>,
			Policy::Add<EngineUniformBuffer, Policy::EPermission::Write>,
			Policy::Add<Window, Policy::EPermission::Write>,
			Policy::Add<DebugLineDrawer, Policy::EPermission::Write>,
			Policy::Add<GlfwFacade, Policy::EPermission::Write>,
			Policy::Add<OpenGLFacade, Policy::EPermission::Write>>>
	, public Debugged
{
public:
	Renderer(Dependencies&& aDependencies) noexcept;

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

REGISTER_SYSTEM(Renderer);
