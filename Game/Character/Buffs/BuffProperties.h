#pragma once
#include "Character/Stats/CharacterStatsFunctionality.h"
#include "Typedefs.h"

class CharacterFunctionality;
class BuffInstance;
class BuffBlueprintPropertyInstance;

class BuffBlueprintProperty
{
public:
	virtual void Apply(CharacterFunctionality& aCharacter) { aCharacter; };
	virtual void Remove(CharacterFunctionality& aCharacter) { aCharacter; };

	virtual void ApplyOnBuff(BuffInstance& aBuff) { aBuff; };

	virtual void Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) { aBuffPropertyInstance; };

	virtual bool BuffShouldDie(const BuffBlueprintPropertyInstance& aBuffPropertyInstance) const { aBuffPropertyInstance; return false; }

	virtual void Inspect() const {}
	virtual void InspectInstance(BuffBlueprintPropertyInstance& aBuffPropertyInstance) const { aBuffPropertyInstance; }

};

class BuffBlueprintPropertyInstance
{
public:
	BuffBlueprintPropertyInstance(BuffBlueprintProperty& aOwningBlueprintProperty, CharacterFunctionality& aCharacter)
		: myBlueprintProperty(aOwningBlueprintProperty), myCharacter(aCharacter) {}

	void Tick();
	void Inspect();
	bool BuffShouldDie() const;

	BuffBlueprintProperty& myBlueprintProperty;
	CharacterFunctionality& myCharacter;
};
