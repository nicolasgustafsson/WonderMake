#pragma once
#include "Components/Component.h"
#include "Typedefs.h"

struct EnemyControllerComponent
	: public SComponent
{
	f32 FollowRangeMin = 50.f;
};