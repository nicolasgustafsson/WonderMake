#pragma once
#include "Utilities/BezierCurve.h"
enum class ESwingStages
{
	PreSwing,
	Swing,
	BackSwing
};

struct SSwing
{
	BezierCurve SwingPath;

	f32 ChargeTime = 0.4f;
	f32 SwingTime = 0.2f;
	f32 StepLength = 60.f;

	f32 DamageMultipiler = 1.0f;
};
