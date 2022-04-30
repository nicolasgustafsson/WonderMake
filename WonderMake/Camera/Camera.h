#pragma once
#include "System/System.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderNodeGraph/RenderNodeGraph.h"
#include "Camera/Display.h"
#include "Utilities/Color.h"

class Camera final : public NonCopyable, public NonMovable
{
public:
	Camera(const std::string& aName, const bool aIsFirst = false);

	Camera(Camera&& aOther) = default;
	void Update();

	void SetPosition(const SVector2f aPosition);
    SVector2f GetPosition() const;

	void FinishFrame();
	void FinishDebugFrame();

	[[nodiscard]] SVector2f ConvertToWorldPosition(const SVector2f aScreenPosition) const;

	const SMatrix33f& GetViewMatrix() const noexcept { return myViewMatrix; }

	[[nodiscard]] SRadianF32 GetRotation() const noexcept { return myRotation; }
	[[nodiscard]] f32 GetScale() const noexcept { return myScale; }

	void Inspect();

	[[nodiscard]] Display* GetFirstDisplay();
	[[nodiscard]] const Display* GetFirstDisplay() const;
	[[nodiscard]] Display* GetFocusedDisplay();
	[[nodiscard]] const Display* GetFocusedDisplay() const;

private:
	SVector2f myPosition;
	
	SRadianF32 myRotation = 0.f;
	f32 myScale = 1.0f;
	SMatrix33f myViewMatrix;

	std::unordered_map<std::string, Display> myDisplays;

	std::string myName;

	CameraUniformBuffer myCameraBuffer;

	const SColor ClearColor = SColor::Grey();
};

