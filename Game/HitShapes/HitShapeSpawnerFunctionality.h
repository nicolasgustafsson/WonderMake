#pragma once
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

enum class EDirection
{
	Left,
	Right
};

//[Nicos]: Basically just a bunch of helper functions for spawning hitshapes
class HitShapeSpawnerFunctionality
	: public Functionality<
	HitShapeSpawnerFunctionality,
	Policy::Set<
	Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
	Policy::Add<SLevelDenizenComponent, Policy::EPermission::Write>>>
{
public:
	HitShapeSpawnerFunctionality(Object& aOwner);
	void SpawnPunch(const f32 aLength, const f32 aDelay, const f32 aDuration);
};

