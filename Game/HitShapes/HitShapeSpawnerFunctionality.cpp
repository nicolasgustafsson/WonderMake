#include "pch.h"
#include "HitShapeSpawnerFunctionality.h"
#include "HitShapeFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "Levels/LevelFunctionality.h"
#include <Randomizer/Randomizer.h>
#include "Utilities/Math.h"
#include "Utility/Constants.h"

HitShapeSpawnerFunctionality::HitShapeSpawnerFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void HitShapeSpawnerFunctionality::SpawnPunch(const f32 aLength, const f32 aDelay, const f32 aDuration, const f32 aWidth, const f32 aDamage)
{
	Object hitShape;

	TransformFunctionality& transform = Get<TransformFunctionality>();

	HitShapeFunctionality& hitShapeFunctionality = hitShape.Add<HitShapeFunctionality>();
	
	SVector2f characterPosition = transform.GetPosition();
	
	EDirection direction = SystemPtr<Randomizer>()->SelectOne<EDirection, EDirection::Left, EDirection::Right>();

	const f32 directionSign = direction == EDirection::Right ? 1.f : -1.f;

	f32 offset = SystemPtr<Randomizer>()->GetRandomNumber(10.f, 40.f) * directionSign;
	f32 control1Offset = SystemPtr<Randomizer>()->GetRandomNumber(0.f, 30.f) * directionSign;
	f32 control2Offset = SystemPtr<Randomizer>()->GetRandomNumber(0.f, 30.f) * directionSign;

	const SVector2f start = characterPosition + transform.GetRightVector() * offset - transform.GetForwardVector() * 20.f;
	const SVector2f end = transform.GetPosition() + transform.GetForwardVector() * aLength;

	SVector2f normal = (end - start).GetNormalized().GetPerpendicularClockWise() * offset;
	SVector2f control1Normal = (end - start).GetNormalized().GetPerpendicularClockWise() * control1Offset;
	SVector2f control2Normal = (end - start).GetNormalized().GetPerpendicularClockWise() * control2Offset;

	const SVector2f control1 = WmMath::Lerp(start, end, 0.33f) + control1Normal;
	const SVector2f control2 = WmMath::Lerp(start, end, 0.66f) + control2Normal;
	BezierCurve curve(start, end, control1, control2);

	const EFaction faction = Get<FactionFunctionality>().GetFaction();
	hitShapeFunctionality.SetFromBezier(curve, aWidth, aDuration, aDelay, aDamage, faction);

	Get<SLevelDenizenComponent>().Level->AddDenizen(std::move(hitShape));
}

void HitShapeSpawnerFunctionality::SpawnSwordSwing(BezierCurve aSwordPath, const f32 aDelay, const f32 aDuration, const f32 aWidth, const f32 aDamage)
{
	Object hitShape;
	TransformFunctionality& transform = Get<TransformFunctionality>();
	HitShapeFunctionality& hitShapeFunctionality = hitShape.Add<HitShapeFunctionality>();

	aSwordPath.Rotate(transform.GetRotation());
	aSwordPath.Offset(transform.GetPosition());

	const EFaction faction = Get<FactionFunctionality>().GetFaction();
	hitShapeFunctionality.SetFromBezier(aSwordPath, aWidth, aDuration, aDelay, aDamage, faction);

	Get<SLevelDenizenComponent>().Level->AddDenizen(std::move(hitShape));
}
