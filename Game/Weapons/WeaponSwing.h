#pragma once
#include "Utilities/BezierCurve.h"

struct SSwing
{
	SSwing();

	f32 mySwingTime = 1.f;

	BezierCurve mySwingPath;
};
