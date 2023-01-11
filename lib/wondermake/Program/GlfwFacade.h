#pragma once
struct GLFWmonitor;
struct GLFWwindow;

typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*GLFWscrollfun)(GLFWwindow*, double, double);

class GlfwFacade
	: public System<
		Policy::Set<>,
		STrait::Set<
			STFoundational,
			STGui,
			STSingleton,
			STThreadsafe,
			STWonderMake>>
{
public:
	GlfwFacade();

	void Terminate();
	void SetWindowShouldClose(GLFWwindow* aWindow, const bool aShouldClose);
	void PollEvents();

	void GetWindowPos(GLFWwindow* aWindow, int* aXPosition, int* aYPosition);
	void GetCursorPos(GLFWwindow* aWindow, double* aXPosition, double* aYPosition);

	void GetWindowSize(GLFWwindow* aWindow, int* aWidth, int* aHeight);
	void SetWindowSize(GLFWwindow* aWindow, i32 aWidth, i32 aHeight);

	GLFWframebuffersizefun SetFramebufferSizeCallback(GLFWwindow* aWindow, GLFWframebuffersizefun aCallback);
	GLFWscrollfun SetScrollCallback(GLFWwindow* aWindow, GLFWscrollfun aCallback);

	void SetWindowUserPointer(GLFWwindow* aWindow, void* aPointer);
	[[nodiscard]] void* GetWindowUserPointer(GLFWwindow* aWindow);

	[[nodiscard]] bool ShouldWindowClose(GLFWwindow* aWindow);
	
	[[nodiscard]] GLFWwindow* GetCurrentContext();
	void MakeContextCurrent(GLFWwindow* aWindow);

	void SetWindowHint(const i32 aHintName, const i32 aHintValue);
	[[nodiscard]] GLFWwindow* CreateGlfwWindow(const i32 aWidth, const i32 aHeight, const char* aTitle, GLFWmonitor* aMonitor, GLFWwindow* sharedContextWindow);

	void SetWindowTitle(GLFWwindow* aWindow, const char* aTitle);

	[[nodiscard]] i32 GetWindowAttrib(GLFWwindow* aWindow, int aAttribute);

	[[nodiscard]] i32 InitializeGlad();

	void SwapBuffers(GLFWwindow* aWindow);

	void SwapInterval(i32 aInterval);

	[[nodiscard]] i32 GetKey(GLFWwindow* aWindow, const i32 aKey);
	[[nodiscard]] const unsigned char* GetJoystickButtons(const i32 aJoystickId, int* aCount);
	[[nodiscard]] i32 GetMouseButton(GLFWwindow* aWindow, const i32 aButton);
};
