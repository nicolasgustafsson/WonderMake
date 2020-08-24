#include "pch.h"
#include "Camera.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Program/Window.h"
#include <GLFW/glfw3.h>
#include <Program/GlfwFacade.h>
#include "Graphics/RenderCommandProcessor.h"
#include <utility>

void Camera::Update()
{
	myViewMatrix.SetPosition(myPosition);

	auto viewInverse = myViewMatrix;

	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotateAroundZ(myRotation);

	viewInverse = rotationMatrix * viewInverse;

	viewInverse.m11 *= myScale;
	viewInverse.m12 *= myScale;
	viewInverse.m22 *= myScale;
	viewInverse.m21 *= myScale;

	viewInverse.Inverse();

	auto& buffer = myCameraBuffer.GetBuffer();

	const auto viewMatrix = viewInverse;

	buffer.ViewMatrix = viewMatrix;

	buffer.CameraPosition = myPosition;
	myCameraBuffer.Update();
}

void Camera::SetPosition(const SVector2f aPosition)
{
	myPosition = aPosition;
}

Camera::Camera(const std::string& aName)
	: myName(aName)
{
	std::string displayName = myName + " " + "Main Display";
	myDisplays.emplace(std::piecewise_construct,
		std::forward_as_tuple(displayName),
		std::forward_as_tuple(displayName, *this));
}

void Camera::FinishDebugFrame()
{
	for (auto& display : myDisplays)
	{
		display.second.FinishDebugFrame();
	}
}

void Camera::FinishFrame()
{
	Update();	

	for (auto& display : myDisplays)
	{
		display.second.FinishFrame();
	}
}

void Camera::Inspect()
{
	ImGui::PushID(this);

	for (auto& display : myDisplays)
	{
		display.second.Inspect();
	}
	ImGui::PopID();
}

SVector2f Camera::ConvertToWorldPosition(const SVector2f aScreenPosition) const 
{
	for (auto& display : myDisplays)
	{
		return display.second.ConvertToWorldPosition(aScreenPosition);
	}

	return SVector2f::Zero();
}
