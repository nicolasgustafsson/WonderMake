#pragma once
#include "Character/Buffs/BuffProperties.h"

class BuffStatChangeProperty : public BuffBlueprintProperty
{
public:
	BuffStatChangeProperty(const SStatChange aStatChange);

	void Apply(CharacterFunctionality& aCharacter) override;
	void Remove(CharacterFunctionality& aCharacter) override;

	virtual void Inspect() const;
private:
	const SStatChange myStatChange;
};
