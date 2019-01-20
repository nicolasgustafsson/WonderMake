#pragma once
#include "System/SystemPtr.h"
#include "Window.h"

class ImguiWrapper
{
public:
	ImguiWrapper();

	void StartFrame();

	void EndFrame();

private:
	SystemPtr<Window> myWindowPtr;
};

