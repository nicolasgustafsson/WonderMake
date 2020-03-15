#pragma once
#include "Character/Buffs/BuffProperties.h"

class BuffLifetimeProperty : public BuffBlueprintProperty
{
public:
	BuffLifetimeProperty(const f32 aLifeTime)
		: BuffBlueprintProperty(), myLifeTime(aLifeTime) {}

	virtual void ApplyOnBuff(BuffInstance& aBuff) override;
	virtual void Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance) override;
	virtual bool BuffShouldDie(const BuffBlueprintPropertyInstance& aBuffPropertyInstance) const;

	virtual void Inspect() const override;
	virtual void InspectInstance(BuffBlueprintPropertyInstance& aBuffPropertyInstance) const override;

private:
	const f32 myLifeTime = 0.f;
};

class BuffLifetimePropertyInstance : public BuffBlueprintPropertyInstance
{
public:
	BuffLifetimePropertyInstance(BuffBlueprintProperty& aOwningBlueprintProperty, CharacterFunctionality& aCharacter, const f32 aLifeTime)
		: BuffBlueprintPropertyInstance(aOwningBlueprintProperty, aCharacter), myLifeLeft(aLifeTime) {}

	f32 myLifeLeft;
};