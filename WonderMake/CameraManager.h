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

protected:
	void Debug() override;

private:
	plf::colony<Camera> myCameras;

};

