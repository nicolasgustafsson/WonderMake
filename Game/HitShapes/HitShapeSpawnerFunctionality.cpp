#include "pch.h"
#include "HitShapeSpawnerFunctionality.h"
#include "HitShapeFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "Levels/LevelFunctionality.h"
#include <Randomizer/Randomizer.h>

HitShapeSpawnerFunctionality::HitShapeSpawnerFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void HitShapeSpawnerFunctionality::SpawnPunch(const f32 aLength)
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

	const SVector2f start = characterPosition + transform.GetRightVector() * offset;
	const SVector2f end = transform.GetPosition() + transform.GetForwardVector() * aLength;

	SVector2f normal = (end - start).GetNormalized().GetPerpendicularClockWise() * offset;
	SVector2f control1Normal = (end - start).GetNormalized().GetPerpendicularClockWise() * control1Offset;
	SVector2f control2Normal = (end - start).GetNormalized().GetPerpendicularClockWise() * control2Offset;

	const SVector2f control1 = start + control1Normal;
	const SVector2f control2 = end + control2Normal;
	BezierCurve curve(start, end, control1, control2);

	hitShapeFunctionality.SetFromBezier(curve, 10.f, 2.f);

	Get<SLevelDenizenComponent>().Level->AddDenizen(std::move(hitShape));
}
