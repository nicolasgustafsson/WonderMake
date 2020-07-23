#pragma once
#include "Buff.h"
#include "Functionalities/ImpulseFunctionality.h"

class BuffBlueprint;

struct SCharacterBuffComponent : public SComponent
{
	plf::colony<BuffInstance> Buffs;
};

class CharacterBuffsFunctionality
	: public Functionality<
		CharacterBuffsFunctionality,
		Policy::Set<
			Policy::Add<SCharacterBuffComponent, Policy::EPermission::Write>,
			Policy::Add<ImpulseFunctionality, Policy::EPermission::Write>>>
{
public:
	CharacterBuffsFunctionality(Object& aOwner);

	void ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint);
	void ClearBuffs();

	bool HasBuff(BuffBlueprint& aBuffBlueprint) const;
	void Tick();

	void Inspect();
};

REGISTER_COMPONENT(SCharacterBuffComponent);
REGISTER_FUNCTIONALITY(CharacterBuffsFunctionality);
