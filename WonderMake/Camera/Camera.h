#pragma once
#include "Utilities/Singleton.h"

class Camera : public Singleton<Camera>
{
public:
	Camera();
	~Camera();

	void Update();

	void Debug();

	void SetViewportSize(const SVector2i aViewportSize);

private:
	SVector2f myPosition;
	float myRotation;
	float myScale = 1.0f;
	SMatrix33f myProjectionMatrix;
	SMatrix33f myViewMatrix;
};

