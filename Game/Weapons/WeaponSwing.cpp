#include "pch.h"
#include "WeaponSwing.h"

SSwing::SSwing()
{
	mySwingPath = BezierCurve(
		{ -25.f, 25.f },
		{ 25.f, 25.f },
		{ -25.f, -30.f },
		{ 0.f, -100.f });
}
