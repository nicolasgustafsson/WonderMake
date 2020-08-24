#pragma once
#include "System/System.h"
#include "System/SystemPtr.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderNodeGraph/RenderNodeGraph.h"
#include "Camera/Display.h"

class Camera final : public NonCopyable, public NonMovable
{
public:
	Camera(const std::string& aName);

	Camera(Camera&& aOther) = default;
	void Update();

	void SetPosition(const SVector2f aPosition);

	void FinishFrame();
	void FinishDebugFrame();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aScreenPosition) const;

	const SMatrix33f& GetViewMatrix() const noexcept { return myViewMatrix; }

	[[nodiscard]] f32 GetRotation() const noexcept { return myRotation; }
	[[nodiscard]] f32 GetScale() const noexcept { return myScale; }

	void Inspect();
private:
	SVector2f myPosition;
	
	//[Nicos]: change this to proper rotation
	f32 myRotation = 0.f;
	f32 myScale = 1.0f;
	SMatrix33f myViewMatrix;

	std::unordered_map<std::string, Display> myDisplays;

	std::string myName;

	CameraUniformBuffer myCameraBuffer;

	const SColor ClearColor = SColor::Grey;
};

