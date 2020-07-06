#include "pch.h"
#include "HitShapeFunctionality.h"
#include "Utilities/TimeKeeper.h"
#include "Collision/CollisionSystem.h"
#include "UtilityFunctionalities/FactionFunctionality.h"
#include "Utilities/MathUtility.h"

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

		hitShapeComponent.RenderObject->Render();
		return;
	}
	
	const f32 previousTime = myTime;

	myTime += aDeltaTime;
	hitShapeComponent.RenderObject->SetAnticipationProgress(GetProgressFromTime(myTime));
	hitShapeComponent.RenderObject->SetHitProgress(GetHitProgressFromTime(myTime));

	hitShapeComponent.RenderObject->Render();

	const f32 previousHitProgress = MathUtility::Clamp(0.1f, 0.9f, GetHitProgressFromTime(previousTime));
	const f32 currentHitProgress = MathUtility::Clamp(0.1f, 0.9f, GetHitProgressFromTime(myTime));

	if (previousHitProgress == currentHitProgress)
		return;

	const SVector2f previousLocation = hitShapeComponent.Bezier.GetConstantLocationAt(previousHitProgress);
	const SVector2f currentLocation = hitShapeComponent.Bezier.GetConstantLocationAt(currentHitProgress);

	SystemPtr<CollisionSystem>()->OverlapLineAgainstFunctionality<CharacterFunctionality>(previousLocation, currentLocation, myWidth * 0.5f, [&](CharacterFunctionality& aCharacter, const auto&&)
		{
			if (aCharacter.Get<FactionFunctionality>().GetFaction() == myFaction)
				return;

			aCharacter.Damage(20);
		});
}

void HitShapeFunctionality::SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime, const f32 aDelay, const EFaction aFaction)
{
	myFaction = aFaction;
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

f32 HitShapeFunctionality::GetHitProgressFromTime(const f32 aTime) const noexcept
{
	return GetProgressFromTime(aTime - AnticipationDuration);
}
