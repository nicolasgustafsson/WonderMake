#include "pch.h"
#include "CharacterSkeleton.h"

CharacterSkeletonFunctionality::CharacterSkeletonFunctionality(Object& aOwner)
	:Super(aOwner)
{

}

void CharacterSkeletonFunctionality::Tick()
{
	UpdateFoot(Get<SCharacterSkeletonComponent>().LeftFoot);
	UpdateFoot(Get<SCharacterSkeletonComponent>().RightFoot);
	DebugDraw();
}

void CharacterSkeletonFunctionality::DebugDraw() const
{
	WmDrawDebugSphere(GetPelvisPosition(), 5.f, { 0.f, 0.f, 0.f, 1.0f }, 5);
	WmDrawDebugSphere(GetLeftFootPosition(), 5.f, SColor::Purple, 5);

	if (Get<SCharacterSkeletonComponent>().LeftFoot.CurrentStep)
	{
		WmDrawDebugSphere(Get<SCharacterSkeletonComponent>().LeftFoot.CurrentStep->StartLocation, 3.f, SColor::Orange, 5);
		WmDrawDebugSphere(Get<SCharacterSkeletonComponent>().LeftFoot.CurrentStep->TargetLocation, 3.f, SColor::Orange, 5);
		WmDrawDebugLine(Get<SCharacterSkeletonComponent>().LeftFoot.CurrentStep->StartLocation, Get<SCharacterSkeletonComponent>().LeftFoot.CurrentStep->TargetLocation, SColor::Orange);
		
	}
	//WmDrawDebugSphere(GetRightFootPosition(), 5.f, SColor::Purple, 5);
}

void CharacterSkeletonFunctionality::UpdateFoot(SFootAnimation& aFoot)
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	if (ShouldTakeAStep(aFoot))
	{
		BeginStep(aFoot);
	}

	if (aFoot.CurrentStep)
	{
		UpdateStep(aFoot);
	}
}

SVector2f CharacterSkeletonFunctionality::GetPelvisPosition() const
{
	return Get<TransformFunctionality>().GetPosition();
}

SVector2f CharacterSkeletonFunctionality::GetLeftFootPosition() const
{
	return Get<SCharacterSkeletonComponent>().LeftFoot.WorldLocation;
}

SVector2f CharacterSkeletonFunctionality::GetRightFootPosition() const
{
	return Get<SCharacterSkeletonComponent>().RightFoot.WorldLocation;
}

bool CharacterSkeletonFunctionality::IsInStep(const SFootAnimation& aFoot) const
{
	return aFoot.CurrentStep.has_value();
}

bool CharacterSkeletonFunctionality::ShouldTakeAStep(const SFootAnimation& aFoot) const
{
	if (aFoot.CurrentStep)
		return false;

	SVector2f characterPosition = Get<TransformFunctionality>().GetPosition();

	constexpr const f32 requiredDistanceForStep = 25.f;

	return (aFoot.WorldLocation.DistanceTo(characterPosition) > requiredDistanceForStep);
}

void CharacterSkeletonFunctionality::BeginStep(SFootAnimation& aFoot)
{
	SFootAnimation::SStep step;

	SVector2f characterPosition = Get<TransformFunctionality>().GetPosition();
	SVector2f characterVelocity = Get<DefaultMovementFunctionality>().Get<SDefaultMovementComponent>().CurrentVelocity;
	
	step.TargetLocation = characterPosition + characterVelocity * 0.15f;
	step.StartLocation = aFoot.WorldLocation;
	step.StepProgress = 0.f;
	
	aFoot.CurrentStep = step;
}

void CharacterSkeletonFunctionality::UpdateStep(SFootAnimation& aFoot)
{	
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	aFoot.CurrentStep->StepProgress += deltaTime * 12.f;

	aFoot.WorldLocation = aFoot.CurrentStep->StartLocation + (aFoot.CurrentStep->TargetLocation - aFoot.CurrentStep->StartLocation) * aFoot.CurrentStep->StepProgress;

	if (aFoot.CurrentStep->StepProgress >= 1.0f)
	{
		aFoot.CurrentStep.reset();
		return;
	}
}
