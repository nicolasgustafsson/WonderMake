#pragma once
#include "Buff.h"

class BuffBlueprint;

struct SCharacterBuffComponent : public SComponent
{
	plf::colony<Buff> Buffs;
};

class CharacterBuffsFunctionality : public Functionality<CharacterBuffsFunctionality, SCharacterBuffComponent>
{
public:
	CharacterBuffsFunctionality(Object& aOwner);

	void ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint);

	bool HasBuff(BuffBlueprint& aBuffBlueprint) const;
	void Tick();
};

