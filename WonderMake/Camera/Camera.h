#pragma once
#include "System/System.h"
#include "System/SystemPtr.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"
#include <Utilities/Debugging/Debugged.h>

class Camera final
	: public System
	, public Debugged
{
public:
	Camera() = default;
	~Camera() = default;

	void Update();

	void SetViewportSize(const SVector2i aViewportSize);

private:
	virtual void Debug() override;

	SystemPtr<EngineUniformBuffer> myEngineBufferPtr;
	SVector2f myPosition;
	float myRotation;
	float myScale = 1.0f;
	SMatrix33f myProjectionMatrix;
	SMatrix33f myViewMatrix;
};

