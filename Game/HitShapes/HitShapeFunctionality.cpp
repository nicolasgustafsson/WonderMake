#include "pch.h"
#include "HitShapeFunctionality.h"

HitShapeFunctionality::HitShapeFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void HitShapeFunctionality::Tick()
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	if (!hitShapeComponent.RenderObject)
		return;

	hitShapeComponent.RenderObject->Render();
}

void HitShapeFunctionality::SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime)
{
	Get<TimeToLiveFunctionality>().SetTimeToLive(aLifetime);
	Get<SHitShapeComponent>().Bezier = aCurve;

	//WmDrawDebugBezier(aCurve, SColor::Red, 10, aLifetime);

	Get<SHitShapeComponent>().RenderObject.emplace(aCurve, 10, aWidth);
}
