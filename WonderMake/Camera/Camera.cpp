#include "pch.h"
#include "Camera.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Program/Window.h"
#include <GLFW/glfw3.h>

void Camera::Update()
{
	myViewMatrix.SetPosition(myPosition);

	auto viewInverse = myViewMatrix;

	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotationZ(myRotation);

	viewInverse = rotationMatrix * viewInverse;

	viewInverse[0][0] *= myScale;
	viewInverse[1][0] *= myScale;
	viewInverse[1][1] *= myScale;
	viewInverse[0][1] *= myScale;

	viewInverse.FastInverse();

	auto& buffer = Get<EngineUniformBuffer>().GetBuffer();

	const auto projectionMatrix = myProjectionMatrix;
	const auto viewMatrix = viewInverse;
	const auto viewProjectionMatrix = viewMatrix * myProjectionMatrix;

	//projectionMatrix.Transpose();
	//viewMatrix.Transpose();
	//viewProjectionMatrix.Transpose();

	buffer.ProjectionMatrix = projectionMatrix;
	buffer.ViewMatrix = viewMatrix;
	buffer.ViewProjectionMatrix = viewProjectionMatrix;
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
	myProjectionMatrix[0][0] = 2.0f / aViewportSize.X;
	myProjectionMatrix[1][1] = 2.0f / aViewportSize.Y;

	myProjectionMatrixInverse[0][0] = aViewportSize.X / 2.0f;
	myProjectionMatrixInverse[1][1] = aViewportSize.Y / 2.0f;
	myViewportSize = { aViewportSize.X, aViewportSize.Y };
}

SVector2f Camera::ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept
{
	SMatrix33f view = myViewMatrix;

	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotationZ(myRotation);

	view = rotationMatrix * myViewMatrix;

	view[0][0] /= myScale;
	view[1][0] /= myScale;
	view[1][1] /= myScale;
	view[0][1] /= myScale;

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
