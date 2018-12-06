#pragma once
#include "Graphics/Renderer.h"
#include "Window.h"
#include "ImguiWrapper.h"

class Program
{
public:
	Program();

	void Start();

private:

	void Update();
	void SetupCallbacks();
	void OnWindowSizeChanged([[maybe_unused]]GLFWwindow* Window, i32 X, i32 Y);

	//-------KEEP THIS IN ORDER------
	Window myWindow;
	Renderer myRenderer;
	ImguiWrapper myImguiWrapper;
	//-------KEEP THIS IN ORDER------

	static constexpr SVector2<i32> WindowSize = {1600, 900};
};

