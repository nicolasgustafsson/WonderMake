#pragma once
#include "Character/Stats/CharacterStatsFunctionality.h"

class CharacterFunctionality;

class BuffProperty
{
public:
	virtual void Apply(CharacterFunctionality& aCharacter) = 0;
	virtual void Remove(CharacterFunctionality& aCharacter) { aCharacter; };

	virtual void Inspect() const = 0;
};

class BuffStatChangeProperty : public BuffProperty
{
public:
	BuffStatChangeProperty(const SStatChange aStatChange);

	void Apply(CharacterFunctionality& aCharacter) override;
	void Remove(CharacterFunctionality& aCharacter) override;

	virtual void Inspect() const;
private:
	const SStatChange myStatChange;
};