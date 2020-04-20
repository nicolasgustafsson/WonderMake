#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"

struct SFootAnimation
{
	struct SStep
	{
		f32 StepProgress = 0.f;

		SVector2f StartLocation;
		SVector2f TargetLocation;
	};

	std::optional<SStep> CurrentStep;
	SVector2f WorldLocation = { 0.f, 0.f };
};

struct SCharacterSkeletonComponent : public SComponent
{
	SFootAnimation LeftFoot;
	SFootAnimation RightFoot;
};

class CharacterSkeletonFunctionality : public Functionality<CharacterSkeletonFunctionality, TransformFunctionality, CharacterFunctionality, DefaultMovementFunctionality, SCharacterSkeletonComponent>
{
public: 
	CharacterSkeletonFunctionality(Object& aOwner);
	void Tick();
	void DebugDraw() const;

private:
	void UpdateFoot(SFootAnimation& aFoot);

	SVector2f GetPelvisPosition() const;
	SVector2f GetLeftFootPosition() const;
	SVector2f GetRightFootPosition() const;

	bool IsInStep(const SFootAnimation& aFoot) const;
	bool ShouldTakeAStep(const SFootAnimation& aFoot) const;

	void BeginStep(SFootAnimation& aFoot);
	void UpdateStep(SFootAnimation& aFoot);
};

