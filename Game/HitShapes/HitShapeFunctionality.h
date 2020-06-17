#pragma once
#include "Utilities/BezierCurve.h"
#include <UtilityFunctionalities/TimeToLiveFunctionality.h>
#include "HitShapeRenderObject.h"

struct SHitShapeComponent : public SComponent
{
	BezierCurve Bezier;
	std::optional<HitShapeRenderObject> RenderObject;
};

class HitShapeFunctionality
	: public Functionality<
	HitShapeFunctionality,
	Policy::Set<
	Policy::Add<SHitShapeComponent, Policy::EPermission::Write>,
	Policy::Add<TimeToLiveFunctionality, Policy::EPermission::Write>>>
{
public:
	HitShapeFunctionality(Object& aOwner);

	void Tick();

	void SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime, const f32 aDelay);

private:
	void Start();

	[[nodiscard]] f32 GetProgressFromTime(const f32 aTime) const noexcept;

	f32 AnticipationDuration = 0.5f;
	f32 FadeoutDuration = 1.f;

	f32 myTime = 0.f;
	f32 myDelay = 0.f;

	f32 myActiveDuration = 0.f;

	f32 myWidth = 0.f;
};

