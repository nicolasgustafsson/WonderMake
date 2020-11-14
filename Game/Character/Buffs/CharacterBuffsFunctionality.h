#pragma once
#include "Buff.h"
#include "Functionalities/ImpulseFunctionality.h"

class BuffBlueprint;
class TimeKeeper;

struct SCharacterBuffComponent : public SComponent
{
	plf::colony<BuffInstance> Buffs;
};

class CharacterBuffsFunctionality
	: public Functionality<
		CharacterBuffsFunctionality,
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<SCharacterBuffComponent, PWrite>,
			PAdd<ImpulseFunctionality, PWrite>>>
{
public:
	CharacterBuffsFunctionality(Object& aOwner, Dependencies&& aDependencies);

	void ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint);
	void ClearBuffs();

	bool HasBuff(BuffBlueprint& aBuffBlueprint) const;
	void Tick();

	void Inspect();
};
