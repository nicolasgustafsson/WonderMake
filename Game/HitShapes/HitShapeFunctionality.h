#pragma once
#include "Utilities/BezierCurve.h"
#include <UtilityFunctionalities/TimeToLiveFunctionality.h>
#include "HitShapeRenderObject.h"
#include "Character/CharacterFunctionality.h"
#include "UtilityFunctionalities/FactionFunctionality.h"

class CollisionSystem;
class TimeKeeper;

struct SHitShapeComponent : public SComponent
{
	BezierCurve Bezier;
	std::optional<HitShapeRenderObject> RenderObject;

	f32 AnticipationDuration = 0.5f;
	f32 FadeoutDuration = 1.f;

	f32 Time = 0.f;
	f32 Delay = 0.f;
	f32 Damage = 0.f;

	f32 ActiveDuration = 0.f;

	f32 Width = 0.f;

	std::vector<CharacterFunctionality*> HitCharacters;
};

class HitShapeFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<TimeKeeper, PRead>,
			PAdd<SHitShapeComponent, PWrite>,
			PAdd<FactionFunctionality, PWrite>,
			PAdd<TimeToLiveFunctionality, PWrite>>>
{
public:
	HitShapeFunctionality(Object& aOwner, Dependencies&& aDependencies);

	void Tick();

	void SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime, const f32 aDelay, const f32 aDamage, const EFaction aFaction);

	void SkipAnticipation();

	void SetMainColor(const SColor aColor);

private:
	void Start();

	[[nodiscard]] f32 GetProgressFromTime(const f32 aTime) const noexcept;
	[[nodiscard]] f32 GetHitProgressFromTime(const f32 aTime) const noexcept;
};
