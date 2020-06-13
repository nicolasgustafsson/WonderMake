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

	void SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime);
};

