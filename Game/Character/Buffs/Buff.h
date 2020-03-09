#pragma once
#include "Character/Buffs/BuffProperties.h"

enum class EBuffType
{
	Buff,
	Debuff,
	//ItsComplicated
};

class CharacterFunctionality;

//[Nicos]: basically type data of the buff so we can instance it
class BuffBlueprint final
{
public:
	plf::colony<std::unique_ptr<BuffBlueprintProperty>> myProperties;

	class BuffInstance Instantiate(CharacterFunctionality& aCharacter) const;

	void RemoveFrom(CharacterFunctionality& aCharacter) const;
	//[Nicos]: This should be moved out to a property or similar; the buff isn't necessarily timed
	f32 myDuration = 0.f;

	EBuffType myBuffType;

	void Inspect() const;
};

class BuffInstance final : NonCopyable
{
private:
	friend class BuffBlueprint;

	BuffInstance(const BuffBlueprint& aBuffBlueprint, CharacterFunctionality& aCharacter)
		: myBlueprint(aBuffBlueprint), myTimeLeft(aBuffBlueprint.myDuration), myCharacter(aCharacter)
	{}

public:
	void Inspect();

	void Tick();

	f32 GetEstimatedPercentLeft() const;

	const BuffBlueprint& myBlueprint;
	CharacterFunctionality& myCharacter;

	f32 myTimeLeft;

	plf::colony<std::unique_ptr<BuffBlueprintPropertyInstance>> myPropertyInstances;
};

