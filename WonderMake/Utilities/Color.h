#pragma once
#include "Imgui/imgui.h"

struct SColor
{
public:
	float R = 1.0f;
	float G = 1.0f;
	float B = 1.0f;
	float A = 1.0f;

	operator ImColor() { return ImColor(R, G, B, A); }
	operator ImVec4() { return ImVec4(R, G, B, A); }

	const static SColor CornflowerBlue;
	const static SColor White;
	const static SColor Yellow;
	const static SColor Red;
	const static SColor Green;
	const static SColor Blue;
};

