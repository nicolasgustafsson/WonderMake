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

	const f32 aDeltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	if (myDelay > 0.f)
	{
		myDelay -= aDeltaTime;

		if (myDelay < 0.f)
			Start();

		return;
	}
	
	myTime += aDeltaTime;
	hitShapeComponent.RenderObject->SetAnticipationProgress(GetProgressFromTime(myTime));
	hitShapeComponent.RenderObject->SetHitProgress(GetProgressFromTime(myTime - AnticipationDuration));

	hitShapeComponent.RenderObject->Render();
}

void HitShapeFunctionality::SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime, const f32 aDelay)
{
	Get<TimeToLiveFunctionality>().SetTimeToLive(aLifetime + aDelay + AnticipationDuration + FadeoutDuration);
	Get<SHitShapeComponent>().Bezier = aCurve;
	myWidth = aWidth;

	myDelay = aDelay;

	myActiveDuration = aLifetime;

	//WmDrawDebugBezier(aCurve, SColor::Red, 30, aLifetime);

	Get<SHitShapeComponent>().RenderObject.emplace(Get<SHitShapeComponent>().Bezier, 30, myWidth);
	Get<SHitShapeComponent>().RenderObject->SetHitDelay(myDelay);
}

void HitShapeFunctionality::Start()
{
	myTime += (-myDelay);

}

f32 HitShapeFunctionality::GetProgressFromTime(const f32 aTime) const noexcept
{
	if (myActiveDuration == 0.f)
		return 0.f;

	return aTime / myActiveDuration;
}
