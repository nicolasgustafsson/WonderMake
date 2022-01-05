#pragma once
#include <System/System.h>

class GlfwFacade;
struct GLFWwindow;

class Window
	: public System<
		Policy::Set<
			PAdd<GlfwFacade, PWrite>>,
		STrait::Set<
			STGui>>
{
public:
	Window();

	void Update();

	bool IsValid() const;

	SVector2i GetSize() const;

	GLFWwindow* myGlfwWindow = nullptr;
private:
	SVector2i mySize;

};
