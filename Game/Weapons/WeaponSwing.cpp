#include "pch.h"
#include "WeaponSwing.h"
#include "Randomizer/Randomizer.h"

SSwing::SSwing()
{
	IsMirrored = SystemPtr<Randomizer>()->GetRandomBool();

	const f32 HorizontalMultiplier = IsMirrored ? -1.f : 1.f;

	SwingPath = BezierCurve(
		{ -25.f * HorizontalMultiplier, 25.f },
		{ 25.f * HorizontalMultiplier, 25.f },
		{ -25.f * HorizontalMultiplier, -30.f },
		{ 0.f, -100.f });
}
