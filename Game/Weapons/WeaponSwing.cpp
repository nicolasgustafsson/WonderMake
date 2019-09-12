#include "pch.h"
#include "WeaponSwing.h"

SSwing::SSwing()
{
	mySwingPath.myStart = { -25.f, 25.f };
	mySwingPath.myFirstControl = { -25.f, -30.f };
	mySwingPath.mySecondControl = { 0.f, -100.f };
	mySwingPath.myEnd = { 25.f, 25.f };
}
