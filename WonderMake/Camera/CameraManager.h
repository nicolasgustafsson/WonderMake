#pragma once

#include "Camera/Camera.h"

template<typename TResource>
class ResourceSystem;

class RenderNodeGraph;

template class ResourceSystem<RenderNodeGraph>;
class OpenGLFacade;
class Window;

class CameraManager
	: public System<
		Policy::Set<
			PAdd<OpenGLFacade, PWrite>,
			PAdd<ResourceSystem<RenderNodeGraph>, PWrite>,
			PAdd<Window, PRead>>,
		STrait::Set<
			STGui>>
	, public Debugged
{
public:
	CameraManager();

	Camera& GetMainCamera();

	void FinishDebugFrame();

	void FinishFrame();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aScreenPosition) const;

	[[nodiscard]] bool AnyDisplayIsFocused() const;

	void SetViewportSize(const SVector2i aViewportSize);
protected:
	void Debug() override;

	[[nodiscard]] const Display* GetFocusedDisplay() const;
	[[nodiscard]] Display* GetFocusedDisplay();

private:
	plf::colony<Camera> myCameras;

};

