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

//[Nicos]: basically type data of the buff so we can instance it
class BuffBlueprint final
{
public:
	plf::colony<std::unique_ptr<BuffProperty>> myProperties;

	class Buff Instantiate(CharacterFunctionality& aCharacter) const;

	void RemoveFrom(CharacterFunctionality& aCharacter) const;
	//[Nicos]: This should be moved out to a property or similar; the buff isn't necessarily timed
	f32 myDuration = 0.f;

	void Inspect() const;
};

class Buff final : NonCopyable
{
private:
	friend class BuffBlueprint;

	Buff(const BuffBlueprint& aBuffBlueprint, CharacterFunctionality& aCharacter)
		: myBlueprint(aBuffBlueprint), myTimeLeft(aBuffBlueprint.myDuration), myCharacter(aCharacter)
	{}

public:
	void Inspect();

	f32 GetEstimatedPercentLeft() const;

	const BuffBlueprint& myBlueprint;
	CharacterFunctionality& myCharacter;

	f32 myTimeLeft;
};

