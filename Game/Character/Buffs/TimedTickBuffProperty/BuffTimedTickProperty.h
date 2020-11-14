#pragma once
#include "Character/Buffs/BuffProperties.h"

class CharacterEffect;

class BuffTimedTickProperty : public BuffBlueprintProperty
{
public:
	BuffTimedTickProperty(const f32 aTimeBetweenTicks);
	virtual void ApplyOnBuff(BuffInstance& aBuff) override;

	virtual void Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance, const f32 aDeltaTime) override final;

	virtual void TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) = 0;

	virtual void Inspect() const override;

	virtual void InspectInstance(BuffBlueprintPropertyInstance& aBuffPropertyInstance) const override;

public:
	f32 myTimeBetweenTicks = 1.f;
};

class BuffTimedTickPropertyInstance : public BuffBlueprintPropertyInstance
{
public:
	BuffTimedTickPropertyInstance(BuffBlueprintProperty& aOwningBlueprintProperty, CharacterFunctionality& aCharacter)
		: BuffBlueprintPropertyInstance(aOwningBlueprintProperty, aCharacter) {}

	f32 myTimeUntilTick = 0.f;
};

class BuffEffectOverTimeProperty : public BuffTimedTickProperty
{
public:
	BuffEffectOverTimeProperty(const f32 aTimeBetweenTicks, CharacterEffect& aCharacterEffect)
		: BuffTimedTickProperty(aTimeBetweenTicks), myCharacterEffect(aCharacterEffect) {}

	virtual void TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override;

	virtual void Inspect() const override;

private:
	CharacterEffect& myCharacterEffect;
};
