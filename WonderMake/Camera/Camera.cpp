#include "pch.h"
#include "Camera.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Program/Window.h"
#include <GLFW/glfw3.h>

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

	auto& buffer = myEngineBufferPtr->GetBuffer();

	const auto projectionMatrix = myProjectionMatrix;
	const auto viewMatrix = viewInverse;
	const auto viewProjectionMatrix = viewMatrix * myProjectionMatrix;

	//projectionMatrix.Transpose();
	//viewMatrix.Transpose();
	//viewProjectionMatrix.Transpose();

	buffer.ProjectionMatrix = projectionMatrix;
	buffer.ViewMatrix = viewMatrix;
	buffer.ViewProjectionMatrix = viewProjectionMatrix;

	buffer.Resolution = myViewportSize;
	buffer.CameraPosition = myPosition;
}

void Camera::Debug()
{
	ImGui::Begin("Camera Debug Tool");

	ImGui::SliderFloat2("Camera Position", &myPosition.X, -1000.f, 1000.f);
	ImGui::SliderFloat("Camera Rotation", &myRotation, -3.1415f, 3.1415f);
	ImGui::SliderFloat("Camera Scale", &myScale, 0.0f, 10.0f, "%.3f", 3.0f);

	ImGui::End();
}

void Camera::SetViewportSize(const SVector2i aViewportSize) noexcept
{
	myProjectionMatrix.m11 = 2.0f / aViewportSize.X;
	myProjectionMatrix.m22 = 2.0f / aViewportSize.Y;

	myProjectionMatrixInverse.m11 = aViewportSize.X / 2.0f;
	myProjectionMatrixInverse.m22 = aViewportSize.Y / 2.0f;
	myViewportSize = { aViewportSize.X, aViewportSize.Y };
}

SVector2f Camera::ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept
{
	SMatrix33f view = myViewMatrix;

	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotateAroundZ(myRotation);

	view = rotationMatrix * myViewMatrix;

	view.m11 /= myScale;
	view.m12 /= myScale;
	view.m22 /= myScale;
	view.m21 /= myScale;

	SVector2f offsetScreenPosition = aWindowPosition - myImguiWindowOffset;
	offsetScreenPosition -= myViewportSize / 2.f;
	SMatrix33f screenPositionMatrix;
	screenPositionMatrix.SetPosition(offsetScreenPosition);
	
	screenPositionMatrix *= view;

	return screenPositionMatrix.GetPosition();
}

void Camera::SetImguiWindowOffset(const SVector2f aImguiOffset) noexcept
{
	myImguiWindowOffset = aImguiOffset;
}

void Camera::SetPosition(const SVector2f aPosition)
{
	myPosition = aPosition;
}
