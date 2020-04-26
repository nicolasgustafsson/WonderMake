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
	Camera() : Debugged("Camera Settings") {}
	void Update();

	void SetViewportSize(const SVector2i aViewportSize) noexcept;
	void SetImguiWindowOffset(const SVector2f aImguiOffset) noexcept;

	void SetPosition(const SVector2f aPosition);

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept;
private:
	virtual void Debug() override;

	SystemPtr<EngineUniformBuffer> myEngineBufferPtr;
	SVector2f myPosition;
	SVector2f myImguiWindowOffset;
	float myRotation = 0.f;
	float myScale = 1.0f;
	SMatrix33f myProjectionMatrix;
	SMatrix33f myProjectionMatrixInverse;
	SMatrix33f myViewMatrix;
	SVector2f myViewportSize;
};

