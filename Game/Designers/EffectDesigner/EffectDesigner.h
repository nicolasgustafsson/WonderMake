#pragma once

class CharacterEffect;

enum class EEffectType
{
	Positive,
	Negative
};

struct SEffectRequirements
{
	std::optional<EEffectType> Type;
	std::optional<f32> Strength;
};

struct SEffectDesign
{
	EEffectType Type;
	f32 Strength;
	std::unique_ptr<CharacterEffect> Effect;
};

class CharacterEffect;

class EffectDesigner : public System
{
public:
	CharacterEffect& DesignCharacterEffect(SEffectRequirements aRequirements);

private:
	std::unique_ptr<CharacterEffect> DecideEffect(const SEffectDesign& aExistingDesign) const;
	std::unique_ptr<CharacterEffect> DecidePositiveEffect(const SEffectDesign& aExistingDesign) const;
	std::unique_ptr<CharacterEffect> DecideNegativeEffect(const SEffectDesign& aExistingDesign) const;
	EEffectType DecideEffectType() const;
	f32 DecideEffectStrength() const;

	plf::colony<std::unique_ptr<CharacterEffect>> myEffects;
};

