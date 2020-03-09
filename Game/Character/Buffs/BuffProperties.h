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

	virtual void Inspect() const = 0;
};

class BuffBlueprintPropertyInstance
{
public:
	BuffBlueprintPropertyInstance(BuffBlueprintProperty& aOwningBlueprintProperty, CharacterFunctionality& aCharacter)
		: myBlueprintProperty(aOwningBlueprintProperty), myCharacter(aCharacter) {}

	void Tick();

	BuffBlueprintProperty& myBlueprintProperty;
	CharacterFunctionality& myCharacter;
};

class BuffStatChangeBlueprintProperty : public BuffBlueprintProperty
{
public:
	BuffStatChangeBlueprintProperty(const SStatChange aStatChange);

	void Apply(CharacterFunctionality& aCharacter) override;
	void Remove(CharacterFunctionality& aCharacter) override;

	virtual void Inspect() const;
private:
	const SStatChange myStatChange;
};

class BuffBlueprintTickPropertyInstance : public BuffBlueprintPropertyInstance
{
public:
	BuffBlueprintTickPropertyInstance(BuffBlueprintProperty& aOwningBlueprintProperty, CharacterFunctionality& aCharacter)
		: BuffBlueprintPropertyInstance(aOwningBlueprintProperty, aCharacter) {}
	f32 myTimeUntilTick = 0.f;
};

class BuffTimedTickingBlueprintProperty : public BuffBlueprintProperty
{
public:
	BuffTimedTickingBlueprintProperty(const f32 aTimeBetweenTicks);
	virtual void ApplyOnBuff(BuffInstance& aBuff) override;

	virtual void Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override final;

	virtual void TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) = 0;

public:
	f32 myTimeBetweenTicks = 1.3f;
};

class BuffDamageOverTimeBlueprintProperty : public BuffTimedTickingBlueprintProperty
{
public:
	BuffDamageOverTimeBlueprintProperty(const f32 aDamagePerTick, const f32 aTimeBetweenTicks);

	virtual void Inspect() const;

	virtual void TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override;

private:
	f32 myDamagePerTick;
};