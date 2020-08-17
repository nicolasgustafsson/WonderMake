#pragma once
#include "System/System.h"
#include "System/SystemPtr.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"
#include "Graphics/RenderTarget.h"

class Camera final
{
public:
	Camera(std::string aName);

	Camera(Camera&& aOther) = default;
	void Update();

	void SetViewportSize(const SVector2i aViewportSize) noexcept;
	void SetImguiWindowOffset(const SVector2f aImguiOffset) noexcept;

	void SetPosition(const SVector2f aPosition);

	void FinishFrame();
	void FinishDebugFrame();

	void BindAsTexture();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept;
private:

	SystemPtr<EngineUniformBuffer> myEngineBufferPtr;
	SVector2f myPosition;
	SVector2f myImguiWindowOffset;
	float myRotation = 0.f;
	float myScale = 1.0f;
	SMatrix33f myProjectionMatrix;
	SMatrix33f myProjectionMatrixInverse;
	SMatrix33f myViewMatrix;
	SVector2f myViewportSize;

	std::string myName;

	RenderTarget myRenderTarget;

	const SColor ClearColor = SColor::Grey;
};

