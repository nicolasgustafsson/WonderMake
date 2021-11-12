#pragma once
#include "RenderObject.h"
#include "SpriteRenderObject.h"
#include "RenderTarget.h"
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Camera/CameraManager.h"
#include "Program/Window.h"
#include "Message/MessageSubscriber.h"
#include "Utilities/Debugging/Debugged.h"
#include "Debugging/DebugLineDrawer.h"
#include "OpenGLFacade.h"
#include "Graphics/RenderCommandProcessor.h"

class GlfwFacade;

class Renderer
	: public System<
		Policy::Set<
			PAdd<EngineUniformBuffer, PWrite>,
			PAdd<Window, PWrite>,
			PAdd<DebugLineDrawer, PWrite>,
			PAdd<GlfwFacade, PWrite>,
			PAdd<CameraManager, PWrite>,
			PAdd<RenderCommandProcessor, PWrite>,
			PAdd<OpenGLFacade, PWrite>,
			PAdd<UniverseSystemCollection<RenderCommandProcessor>, PWrite>>>
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

	bool myDebugWindowHasFocus = true;
};
