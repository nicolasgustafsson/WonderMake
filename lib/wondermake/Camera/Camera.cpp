#include "pch.h"

#include "Camera.h"

#include "Program/GlfwFacade.h"

Camera::Camera(
	OpenGLFacade&	aOpenGlFacade,
	std::string		aName)
	: myName(std::move(aName))
	, myCameraBuffer(aOpenGlFacade)
{}


void Camera::BindBuffer()
{
	auto& buffer = myCameraBuffer.GetBuffer();

	buffer.ViewMatrix = GetViewMatrix();
	buffer.CameraPosition = myPosition.GetRounded();

	myCameraBuffer.Update();
}
