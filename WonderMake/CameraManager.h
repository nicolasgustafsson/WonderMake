#pragma once
#include "Camera/Camera.h"
class CameraManager : public System, public Debugged
{
public:
	CameraManager();

	Camera& GetMainCamera();

	void Update();

	void FinishDebugFrame();

	void FinishFrame();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aScreenPosition);

protected:
	void Debug() override;

private:
	plf::colony<Camera> myCameras;

};

