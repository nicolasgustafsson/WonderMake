#pragma once
struct GLFWmonitor;
struct GLFWwindow;

typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);

class GlfwFacade : public System<>
{
public:
	GlfwFacade();

	void Terminate();
	void SetWindowShouldClose(GLFWwindow* aWindow, const bool aShouldClose);
	void PollEvents();

	void GetWindowPos(GLFWwindow* aWindow, int* aXPosition, int* aYPosition);
	void GetCursorPos(GLFWwindow* aWindow, double* aXPosition, double* aYPosition);

	void SetCursorPos(GLFWwindow* aWindow, SVector2f aPosition);

	void HideMouse(GLFWwindow* aWindow);
	void ShowMouse(GLFWwindow* aWindow);

	void SetMouseVisibility(GLFWwindow* aWindow, const bool aIsVisible);

	void SetFramebufferSizeCallback(GLFWwindow* aWindow, GLFWframebuffersizefun aCallback);

	void SetWindowUserPointer(GLFWwindow* aWindow, void* aPointer);
	[[nodiscard]] void* GetWindowUserPointer(GLFWwindow* aWindow);

	[[nodiscard]] bool ShouldWindowClose(GLFWwindow* aWindow);
	
	[[nodiscard]] GLFWwindow* GetCurrentContext();
	void MakeContextCurrent(GLFWwindow* aWindow);

	void SetWindowHint(const i32 aHintName, const i32 aHintValue);
	[[nodiscard]] GLFWwindow* CreateGlfwWindow(const i32 aWidth, const i32 aHeight, const char* aTitle, GLFWmonitor* aMonitor, GLFWwindow* sharedContextWindow);

	[[nodiscard]] i32 InitializeGlad();

	void SwapBuffers(GLFWwindow* aWindow);

	[[nodiscard]] i32 GetKey(GLFWwindow* aWindow, const i32 aKey);
	[[nodiscard]] const unsigned char* GetJoystickButtons(const i32 aJoystickId, int* aCount);
	[[nodiscard]] i32 GetMouseButton(GLFWwindow* aWindow, const i32 aButton);
};
