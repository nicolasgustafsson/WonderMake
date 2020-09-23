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

	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotationZ(myRotation);

	viewInverse = rotationMatrix * viewInverse;

	viewInverse[0][0] *= myScale;
	viewInverse[0][1] *= myScale;
	viewInverse[1][1] *= myScale;
	viewInverse[1][0] *= myScale;

	viewInverse.FastInverse();

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

Camera::Camera(const std::string& aName, [[maybe_unused]] const bool aIsFirst)
	: myName(aName)
{
	std::string displayName = myName + " " + "Main Display";
	auto&& displayIt = myDisplays.emplace(std::piecewise_construct,
		std::forward_as_tuple(displayName),
		std::forward_as_tuple(displayName, *this));

	if constexpr (!Constants::IsDebugging)
	{
		if (aIsFirst)
			displayIt.first->second.Focus();
	}
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
	
	ImGui::Text(myName.c_str());

	ImGui::Indent();
	for (auto& display : myDisplays)
	{
		display.second.Inspect();
		ImGui::Separator();
	}

	if (ImGui::Button("Add new display"))
	{
		const std::string displayName = myName + " " + std::to_string(myDisplays.size());
		myDisplays.emplace(std::piecewise_construct,
			std::forward_as_tuple(displayName),
			std::forward_as_tuple(displayName, *this));
	}
	ImGui::Unindent();

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

Display* Camera::GetFocusedDisplay()
{
	for (auto& display : myDisplays)
	{
		if (display.second.HasFocus())
			return &display.second;
	}

	return nullptr;
}

const Display* Camera::GetFocusedDisplay() const
{
	for (auto& display : myDisplays)
	{
		if (display.second.HasFocus())
			return &display.second;
	}

	return nullptr;
}
