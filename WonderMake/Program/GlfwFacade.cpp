
#include "pch.h"
#include "GlfwFacade.h"
#include "GLFW/glfw3.h"
#include <glad/glad.h>
#include <vulkan/vulkan.hpp>
#include "Window.h"

REGISTER_SYSTEM(GlfwFacade);

GlfwFacade::GlfwFacade()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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

vk::SurfaceKHR GlfwFacade::CreateVulkanSurface(vk::Instance instance)
{
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(instance, SystemPtr<Window>()->myGlfwWindow, nullptr, &surface);

	return vk::SurfaceKHR(surface);
}

void GlfwFacade::SetFramebufferSizeCallback(GLFWwindow* aWindow, GLFWframebuffersizefun aCallback)
{
	glfwSetFramebufferSizeCallback(aWindow, aCallback);
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

i32 GlfwFacade::InitializeGlad()
{
	return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void GlfwFacade::SwapBuffers(GLFWwindow* aWindow)
{
	glfwSwapBuffers(aWindow);
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
