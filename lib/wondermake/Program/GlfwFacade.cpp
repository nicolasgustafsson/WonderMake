#include "pch.h"

#include "GlfwFacade.h"

#include <glad/glad.h>

#include "GLFW/glfw3.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(GlfwFacade);

GlfwFacade::GlfwFacade()
{
	glfwInit();
}

void GlfwFacade::Terminate()
{
	glfwTerminate();
}

void GlfwFacade::SetWindowShouldClose(GLFWwindow* aWindow, const bool aShouldClose)
{
	glfwSetWindowShouldClose(aWindow, aShouldClose);
}

void GlfwFacade::PollEvents()
{
	glfwPollEvents();
}

void GlfwFacade::GetWindowPos(GLFWwindow* aWindow, int* aXPosition, int* aYPosition)
{
	glfwGetWindowPos(aWindow, aXPosition, aYPosition);
}

void GlfwFacade::GetCursorPos(GLFWwindow* aWindow, double* aXPosition, double* aYPosition)
{
	glfwGetCursorPos(aWindow, aXPosition, aYPosition);
}

void GlfwFacade::GetWindowSize(GLFWwindow* aWindow, int* aWidth, int* aHeight)
{
	glfwGetWindowSize(aWindow, aWidth, aHeight);
}

void GlfwFacade::SetWindowSize(GLFWwindow* aWindow, i32 aWidth, i32 aHeight)
{
	glfwSetWindowSize(aWindow, aWidth, aHeight);
}

GLFWframebuffersizefun GlfwFacade::SetFramebufferSizeCallback(GLFWwindow* aWindow, GLFWframebuffersizefun aCallback)
{
	return glfwSetFramebufferSizeCallback(aWindow, aCallback);
}

GLFWscrollfun GlfwFacade::SetScrollCallback(GLFWwindow* aWindow, GLFWscrollfun aCallback)
{
	return glfwSetScrollCallback(aWindow, aCallback);
}

void GlfwFacade::SetWindowUserPointer(GLFWwindow* aWindow, void* aPointer)
{
	glfwSetWindowUserPointer(aWindow, aPointer);
}

void* GlfwFacade::GetWindowUserPointer(GLFWwindow* aWindow)
{
	return glfwGetWindowUserPointer(aWindow);
}

bool GlfwFacade::ShouldWindowClose(GLFWwindow* aWindow)
{
	return glfwWindowShouldClose(aWindow);
}

GLFWwindow* GlfwFacade::GetCurrentContext()
{
	return glfwGetCurrentContext();
}

void GlfwFacade::MakeContextCurrent(GLFWwindow* aWindow)
{
	glfwMakeContextCurrent(aWindow);
}

void GlfwFacade::SetWindowHint(const i32 aHintName, const i32 aHintValue)
{
	glfwWindowHint(aHintName, aHintValue);
}

GLFWwindow* GlfwFacade::CreateGlfwWindow(const i32 aWidth, const i32 aHeight, const char* aTitle, GLFWmonitor* aMonitor, GLFWwindow* sharedContextWindow)
{
	return glfwCreateWindow(aWidth, aHeight, aTitle, aMonitor, sharedContextWindow);
}

void GlfwFacade::SetWindowTitle(GLFWwindow* aWindow, const char* aTitle)
{
	glfwSetWindowTitle(aWindow, aTitle);
}

i32 GlfwFacade::GetWindowAttrib(GLFWwindow* aWindow, int aAttribute)
{
	return glfwGetWindowAttrib(aWindow, aAttribute);
}

i32 GlfwFacade::InitializeGlad()
{
	return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void GlfwFacade::SwapBuffers(GLFWwindow* aWindow)
{
	glfwSwapBuffers(aWindow);
}

void GlfwFacade::SwapInterval(i32 aInterval)
{
	glfwSwapInterval(static_cast<int>(aInterval));
}

i32 GlfwFacade::GetKey(GLFWwindow* aWindow, const i32 aKey)
{
	return glfwGetKey(aWindow, aKey);
}

const unsigned char* GlfwFacade::GetJoystickButtons(const i32 aJoystickId, int* aCount)
{
	return glfwGetJoystickButtons(aJoystickId, aCount);
}

i32 GlfwFacade::GetMouseButton(GLFWwindow* aWindow, const i32 aButton)
{
	return glfwGetMouseButton(aWindow, aButton);
}
