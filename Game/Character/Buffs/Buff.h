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

	[[nodiscard]] class BuffInstance Instantiate(CharacterFunctionality& aCharacter) const;

	void RemoveFrom(CharacterFunctionality& aCharacter) const;

	EBuffType myBuffType;

	void Inspect() const;
};

class BuffInstance final : NonCopyable
{
private:
	friend class BuffBlueprint;

	BuffInstance(const BuffBlueprint& aBuffBlueprint, CharacterFunctionality& aCharacter)
		: myBlueprint(aBuffBlueprint), myCharacter(aCharacter)
	{}

public:
	void Inspect();

	void Tick(const f32 aDeltaTime);

	[[nodiscard]] bool ShouldDie() const;

	//f32 GetEstimatedPercentLeft() const;

	const BuffBlueprint& myBlueprint;
	CharacterFunctionality& myCharacter;

	plf::colony<std::unique_ptr<BuffBlueprintPropertyInstance>> myPropertyInstances;
};

