#pragma once

class CharacterFunctionality;
class BuffBlueprint;

class CharacterEffect
{
public:
	virtual void Apply(CharacterFunctionality& aCharacter) = 0;

	virtual void Inspect() = 0;
};

class CharacterEffectHeal : public CharacterEffect
{
public:
	CharacterEffectHeal(const f32 aHealAmount)
		: myHealAmount(aHealAmount) {}

	virtual void Apply(CharacterFunctionality& aCharacter) override;

	virtual void Inspect() override;

	f32 myHealAmount = 0.f;
};

class CharacterEffectDamage : public CharacterEffect
{
public:
	CharacterEffectDamage(const f32 aDamageAmount)
		: myDamageAmount(aDamageAmount) {}

	virtual void Apply(CharacterFunctionality& aCharacter) override;

	virtual void Inspect() override;

	f32 myDamageAmount = 0.f;
};

class CharacterEffectApplyBuff : public CharacterEffect
{
public:
	CharacterEffectApplyBuff(BuffBlueprint& aBuffToApply)
		: myBuffToApply(aBuffToApply) {}

	virtual void Apply(CharacterFunctionality& aCharacter) override;
	
	virtual void Inspect() override;

	BuffBlueprint& myBuffToApply;
};

