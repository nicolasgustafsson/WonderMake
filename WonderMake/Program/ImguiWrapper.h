#pragma once

class GlfwFacade;
class Window;

class ImguiWrapper
	: public Systemus<ImguiWrapper, GlfwFacade, Window>
{
public:
	ImguiWrapper();

	void StartFrame();

	void EndFrame();
};

