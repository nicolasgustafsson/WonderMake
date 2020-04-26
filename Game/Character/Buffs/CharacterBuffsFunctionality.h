#pragma once
#include "Buff.h"
#include "Functionalities/ImpulseFunctionality.h"

class BuffBlueprint;

struct SCharacterBuffComponent : public SComponent
{
	plf::colony<BuffInstance> Buffs;
};

class CharacterBuffsFunctionality : public Functionality<CharacterBuffsFunctionality, SCharacterBuffComponent, ImpulseFunctionality>
{
public:
	CharacterBuffsFunctionality(Object& aOwner);

	void ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint);
	void ClearBuffs();

	bool HasBuff(BuffBlueprint& aBuffBlueprint) const;
	void Tick();

	void Inspect();
};

