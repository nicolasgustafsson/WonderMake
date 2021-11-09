#include "pch.h"
#include "HitShapeFunctionality.h"
#include "Utilities/TimeKeeper.h"
#include "Collision/CollisionSystem.h"
#include "UtilityFunctionalities/FactionFunctionality.h"
#include "Utilities/WmMath.h"
#include "Audio/AudioManager.h"
#include "Utility/Palette.h"

REGISTER_COMPONENT(SHitShapeComponent);
REGISTER_FUNCTIONALITY(HitShapeFunctionality);

void HitShapeFunctionality::Tick()
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	if (!hitShapeComponent.RenderObject)
		return;

	const f32 aDeltaTime = Get<TimeKeeper>().GetDeltaSeconds();
	if (hitShapeComponent.Delay > 0.f)
	{
		hitShapeComponent.Delay -= aDeltaTime;

		if (hitShapeComponent.Delay < 0.f)
			Start();

		hitShapeComponent.RenderObject->Render();
		return;
	}
	
	const f32 previousTime = hitShapeComponent.Time;

	hitShapeComponent.Time += aDeltaTime;
	hitShapeComponent.RenderObject->SetAnticipationProgress(GetProgressFromTime(hitShapeComponent.Time));
	hitShapeComponent.RenderObject->SetHitProgress(GetHitProgressFromTime(hitShapeComponent.Time));

	hitShapeComponent.RenderObject->Render();

	const f32 previousHitProgress = WmMath::Clamp(0.1f, 0.9f, GetHitProgressFromTime(previousTime));
	const f32 currentHitProgress = WmMath::Clamp(0.1f, 0.9f, GetHitProgressFromTime(hitShapeComponent.Time));

	if (previousHitProgress == currentHitProgress)
		return;

	if (previousHitProgress == 0.1f)
	{
		SystemPtr<AudioManager>()->PlayAudio("NodeGraphs/Audio/Swing.json");
	}

	const SVector2f previousBezierPosition = hitShapeComponent.Bezier.GetConstantLocationAt(previousHitProgress);
	const SVector2f currentBezierPosition = hitShapeComponent.Bezier.GetConstantLocationAt(currentHitProgress);

	
	Get<CollisionSystem>().OverlapLineAgainstFunctionality<CharacterFunctionality>(previousBezierPosition, currentBezierPosition, hitShapeComponent.Width * 0.5f, [&](CharacterFunctionality& aCharacter, const auto&&)
		{
			if (aCharacter.Get<FactionFunctionality>().GetFaction() == Get<FactionFunctionality>().GetFaction())
				return;

			if (std::find(hitShapeComponent.HitCharacters.begin(), hitShapeComponent.HitCharacters.end(), &aCharacter) != hitShapeComponent.HitCharacters.end())
				return;

			aCharacter.Damage(static_cast<i32>(hitShapeComponent.Damage));

			hitShapeComponent.HitCharacters.push_back(&aCharacter);
		});
}

void HitShapeFunctionality::SetFromBezier(BezierCurve aCurve, const f32 aWidth, const f32 aLifetime, const f32 aDelay, const f32 aDamage, const EFaction aFaction)
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();

	Get<FactionFunctionality>().SetFaction(aFaction);
	Get<TimeToLiveFunctionality>().SetTimeToLive(aLifetime + aDelay + hitShapeComponent.AnticipationDuration + hitShapeComponent.FadeoutDuration);
	Get<SHitShapeComponent>().Bezier = aCurve;
	hitShapeComponent.Width = aWidth;

	hitShapeComponent.Delay = aDelay;
	hitShapeComponent.ActiveDuration = aLifetime;
	hitShapeComponent.Damage = aDamage;

	//WmDrawDebugBezier(aCurve, SColor::Red, 30, aLifetime);

	Get<SHitShapeComponent>().RenderObject.emplace(Get<SHitShapeComponent>().Bezier, 30, hitShapeComponent.Width);
	Get<SHitShapeComponent>().RenderObject->SetHitDelay(hitShapeComponent.Delay);

	if (aFaction == EFaction::Player)
		SkipAnticipation();

	switch (aFaction)
	{
	case EFaction::Enemy:
		SetMainColor(Palette::EnemyColor);
		break;

	case EFaction::Player:
		SetMainColor(Palette::PlayerColor);
		break;

	default:
		break;
	}
}

void HitShapeFunctionality::SkipAnticipation()
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	
	hitShapeComponent.Time += hitShapeComponent.AnticipationDuration;
}

void HitShapeFunctionality::SetMainColor(const SColor aColor)
{
	if (!Get<SHitShapeComponent>().RenderObject)
		return;

	Get<SHitShapeComponent>().RenderObject->SetMainColor(aColor);
}

void HitShapeFunctionality::Start()
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	hitShapeComponent.Time += (-hitShapeComponent.Delay);
}

f32 HitShapeFunctionality::GetProgressFromTime(const f32 aTime) const noexcept
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	if (hitShapeComponent.ActiveDuration == 0.f)
		return 0.f;

	return aTime / hitShapeComponent.ActiveDuration;
}

f32 HitShapeFunctionality::GetHitProgressFromTime(const f32 aTime) const noexcept
{
	auto& hitShapeComponent = Get<SHitShapeComponent>();
	return GetProgressFromTime(aTime - hitShapeComponent.AnticipationDuration);
}
