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


	f32 myChargeTime = 0.4f;
	f32 mySwingTime = 0.2f;
	f32 myBackswingTime = 0.4f;

	BezierCurve mySwingPath;
};
