#include "pch.h"
#include "GetHurtEffect.h"

void GetHurtEffect::Tick(const f32 aDeltaTime)
{
	WmLog("oww");

	myTimeToLive -= aDeltaTime;
}

bool GetHurtEffect::IsAlive() const
{
	return myTimeToLive > 0.f;
}
