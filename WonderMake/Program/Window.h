#pragma once

struct GLFWwindow;

class Window : NonCopyable
{
public:
	Window() = delete;

	Window(const SVector2<int> Size);
	~Window();

	void Update();

	bool IsValid() const;

	GLFWwindow* myGlfwWindow = nullptr;
};

