#include "pch.h"
#include "HitShapeFunctionality.h"
#include "Utilities/TimeKeeper.h"

HitShapeFunctionality::HitShapeFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void HitShapeFunctionality::Tick()
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	if (!hitShapeComponent.RenderObject)
		return;
	
	myTime += SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	hitShapeComponent.RenderObject->SetAnticipationProgress(myTime * 2.f);

	hitShapeComponent.RenderObject->Render();
}

void HitShapeFunctionality::SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime)
{
	Get<TimeToLiveFunctionality>().SetTimeToLive(aLifetime);
	Get<SHitShapeComponent>().Bezier = aCurve;

	//WmDrawDebugBezier(aCurve, SColor::Red, 30, aLifetime);

	Get<SHitShapeComponent>().RenderObject.emplace(aCurve, 30, aWidth);
}
