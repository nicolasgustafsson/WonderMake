#pragma once
#include "System/System.h"
#include "System/SystemPtr.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"

class Camera 
	: public System
{
public:
	Camera();
	~Camera();

	void Update();

	void SetViewportSize(const SVector2i aViewportSize);

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber mySubscriber;
	SystemPtr<EngineUniformBuffer> myEngineBufferPtr;
	SVector2f myPosition;
	float myRotation;
	float myScale = 1.0f;
	SMatrix33f myProjectionMatrix;
	SMatrix33f myViewMatrix;
};

