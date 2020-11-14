#pragma once
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "UtilityFunctionalities/FactionFunctionality.h"

class HitShapeFunctionality;
class Randomizer;

enum class EDirection
{
	Left,
	Right
};

//[Nicos]: Basically just a bunch of helper functions for spawning hitshapes
class HitShapeSpawnerFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<Randomizer, PWrite>,
			PAdd<FunctionalitySystemDelegate<HitShapeFunctionality>, PWrite>,
			PAdd<TransformFunctionality, PWrite>,
			PAdd<FactionFunctionality, PRead>,
			PAdd<SFactionComponent, PWrite>,
			PAdd<SLevelDenizenComponent, PWrite>>>
{
public:
	HitShapeSpawnerFunctionality(Object& aOwner, Dependencies&& aDependencies);
	void SpawnPunch(const f32 aLength, const f32 aDelay, const f32 aDuration, const f32 aWidth, const f32 aDamage);

	void SpawnSwordSwing(BezierCurve aSwordPath, const f32 aDelay, const f32 aDuration, const f32 aWidth, const f32 aDamage);

private:
};
