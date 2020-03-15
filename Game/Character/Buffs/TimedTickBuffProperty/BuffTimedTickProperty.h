#pragma once
#include "Character/Buffs/BuffProperties.h"


class BuffTimedTickProperty : public BuffBlueprintProperty
{
public:
	BuffTimedTickProperty(const f32 aTimeBetweenTicks);
	virtual void ApplyOnBuff(BuffInstance& aBuff) override;

	virtual void Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override final;

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

//[Nicos]: We will move this functionality into something else later; it can just be an EffectOverTime
class BuffDamageOverTimeProperty : public BuffTimedTickProperty
{
public:
	BuffDamageOverTimeProperty(const f32 aDamagePerTick, const f32 aTimeBetweenTicks)
		: BuffTimedTickProperty(aTimeBetweenTicks), myDamagePerTick(aDamagePerTick) {}

	virtual void TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override;

	virtual void Inspect() const override;

private:
	f32 myDamagePerTick;
};
