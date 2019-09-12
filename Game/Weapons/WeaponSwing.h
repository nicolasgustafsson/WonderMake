#pragma once
#include "Utilities/BezierCurve.h"
enum class SwingStages
{
	PreSwing,
	Swing,
	BackSwing
};

struct SSwing
{
	SSwing();

	f32 mySwingTime = 1.0f;

	BezierCurve mySwingPath;
};
