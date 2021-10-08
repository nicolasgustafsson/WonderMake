#pragma once
#include "ScreenPassRenderObject.h"
#include "Utilities/Debugging/Debugged.h"

class CameraManager;
class GlfwFacade;
class Window;
class EngineUniformBuffer;
class OpenGLFacade;
class RenderCommandProcessor;
class DebugLineDrawer;

class Renderer
	: public Systemus<
			Renderer,
			EngineUniformBuffer,
			Window,
			DebugLineDrawer,
			GlfwFacade,
			CameraManager,
			RenderCommandProcessor,
			OpenGLFacade>
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
