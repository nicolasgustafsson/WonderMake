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

	bool IsMirrored = false;

	f32 ChargeTime = 0.4f;
	f32 SwingTime = 0.2f;
	f32 BackswingTime = 0.4f;
	
	f32 StepLength = 60.f;

	BezierCurve SwingPath;
};
