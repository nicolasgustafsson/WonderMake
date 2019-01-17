#pragma once
#include <System/System.h>

struct GLFWwindow;

class Window : public System
{
public:
	Window();

	~Window();

	void Update();

	bool IsValid() const;

	GLFWwindow* myGlfwWindow = nullptr;
};

