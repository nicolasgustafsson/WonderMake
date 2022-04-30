#pragma once
#include "ScreenPassRenderObject.h"
#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"

class DebugLineDrawer;
class GlfwFacade;
class Window;
class EngineUniformBuffer;
class CameraManager;
class OpenGLFacade;
class RenderCommandProcessor;

class Renderer
	: public System<EngineUniformBuffer, Window, DebugLineDrawer, GlfwFacade, CameraManager, RenderCommandProcessor, OpenGLFacade, UniverseSystemCollection<RenderCommandProcessor>>
	, public Debugged
{
public:
	Renderer() noexcept;

	void SetViewportSize(SVector2<int> WindowSize);

	void StartFrame();

	void FinishFrame();

	bool DebugWindowHasFocus() const noexcept { return myDebugWindowHasFocus; };

private:
	virtual void Debug() override;

	ScreenPassRenderObject myCopyPass;

	bool myDebugWindowHasFocus = true;
};
