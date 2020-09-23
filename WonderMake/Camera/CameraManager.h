#pragma once
#include "Camera/Camera.h"
class CameraManager : public System, public Debugged
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

