#include "stdafx.h"
#include "Camera.h"
#include "Graphics/EngineUniformBuffer.h"


Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::Update()
{
	auto viewInverse = myViewMatrix;

	SMatrix33f rotationMatrix = SMatrix33f::CreateRotateAroundZ(myRotation);

	viewInverse = rotationMatrix * viewInverse;

	viewInverse.m11 *= myScale;
	viewInverse.m12 *= myScale;
	viewInverse.m22 *= myScale;
	viewInverse.m21 *= myScale;

	viewInverse.Inverse();

	auto& buffer = EngineUniformBuffer::Get().GetBuffer();

	auto projectionMatrix = myProjectionMatrix;
	auto viewMatrix = viewInverse;
	auto viewProjectionMatrix = viewMatrix * myProjectionMatrix;

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
	ImGui::SliderFloat("Camera Scale", &myScale, 0.0f, 100.0f, "%.3f", 3.0f);

	ImGui::End();

	myViewMatrix.SetPosition(myPosition);

}

void Camera::SetViewportSize(const SVector2i aViewportSize)
{
	myProjectionMatrix.m11 = 1.0f / aViewportSize.X;
	myProjectionMatrix.m22 = 1.0f / aViewportSize.Y;
}
