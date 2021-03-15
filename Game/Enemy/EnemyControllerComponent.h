#pragma once
#include "Components/Component.h"
#include "Movesets/Moveset.h"
#include "Typedefs.h"

struct EnemyControllerComponent
	: public SComponent
{
	SMoveset Moveset;
	f32 FollowRangeMin = 50.f;
};
