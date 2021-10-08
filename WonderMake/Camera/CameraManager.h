#pragma once
#include "Camera/Camera.h"
class CameraManager : public Systemus<CameraManager>, public Debugged
{
public:
	CameraManager();

	Camera& GetMainCamera();

	void FinishDebugFrame();

	void FinishFrame();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aScreenPosition) const;

	[[nodiscard]] bool AnyDisplayIsFocused() const;

	void SetViewportSize(const SVector2i aViewportSize);

	[[nodiscard]] const Display* GetFocusedDisplay() const;
	[[nodiscard]] Display* GetFocusedDisplay();

protected:
	void Debug() override;

private:
	plf::colony<Camera> myCameras;

};

