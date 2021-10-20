#include "pch.h"

#include "EffectDesigner.h"

#include "Character/Effects/CharacterEffect.h"

#include "Designers/BuffDesigner/BuffDesigner.h"

#include "Randomizer/Randomizer.h"

REGISTER_SYSTEM(EffectDesigner);

CharacterEffect& EffectDesigner::DesignCharacterEffect(SEffectRequirements aRequirements)
{
	SEffectDesign design;

	design.Type = aRequirements.Type.value_or(DecideEffectType());
	design.Strength = aRequirements.Strength.value_or(DecideEffectStrength());
	design.Effect = DecideEffect(design);

	return **(myEffects.insert(std::move(design.Effect)));
}

std::unique_ptr<CharacterEffect> EffectDesigner::DecideEffect(const SEffectDesign& aExistingDesign)
{
	if (aExistingDesign.Type == EEffectType::Positive)
		return DecidePositiveEffect(aExistingDesign);
	else
		return DecideNegativeEffect(aExistingDesign);
}

std::unique_ptr<CharacterEffect> EffectDesigner::DecidePositiveEffect(const SEffectDesign& aExistingDesign)
{
	if (Get<Randomizer>().GetRandomBool())
	{
		return std::make_unique<CharacterEffectHeal>(aExistingDesign.Strength);
	}
	else
	{
		SBuffRequirements requirements;
		requirements.Type = aExistingDesign.Type == EEffectType::Positive ? EBuffType::Buff : EBuffType::Debuff;
		requirements.Strength = aExistingDesign.Strength;

		return std::make_unique<CharacterEffectApplyBuff>(SystemPtr<BuffDesigner>()->DesignBuff(requirements));
	}
}

std::unique_ptr<CharacterEffect> EffectDesigner::DecideNegativeEffect(const SEffectDesign& aExistingDesign)
{
	if (Get<Randomizer>().GetRandomBool())
	{
		return std::make_unique<CharacterEffectDamage>(aExistingDesign.Strength);
	}
	else
	{
		SBuffRequirements requirements;
		requirements.Type = aExistingDesign.Type == EEffectType::Positive ? EBuffType::Buff : EBuffType::Debuff;
		requirements.Strength = aExistingDesign.Strength;

		return std::make_unique<CharacterEffectApplyBuff>(SystemPtr<BuffDesigner>()->DesignBuff({}));
	}
}

EEffectType EffectDesigner::DecideEffectType()
{
	return Get<Randomizer>().SelectOne<EEffectType, EEffectType::Positive, EEffectType::Negative>();
}

f32 EffectDesigner::DecideEffectStrength()
{
	return Get<Randomizer>().GetRandomNumber(50.f, 150.f);
}
