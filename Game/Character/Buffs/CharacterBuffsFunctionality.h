#pragma once
#include "Buff.h"

class BuffBlueprint;
class TimeKeeper;

struct SCharacterBuffComponent : public SComponent
{
	plf::colony<BuffInstance> Buffs;
};

class CharacterBuffsFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<SCharacterBuffComponent, PWrite>>>
{
public:
	void ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint);
	void ClearBuffs();

	bool HasBuff(BuffBlueprint& aBuffBlueprint) const;
	void Tick();

	void Inspect();
};
