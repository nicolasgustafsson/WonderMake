#pragma once
#include "Character/Animation/CharacterSkeleton.h"

struct SCharacterAnimationComponent : public SComponent
{
};

class CharacterAnimationFunctionality : public Functionality<CharacterAnimationFunctionality, SCharacterAnimationComponent, CharacterSkeletonFunctionality>
{
public:
	CharacterAnimationFunctionality(Object& aOwner);

	void Tick();
};

