#pragma once

#include "System/System.h"

#include "Utilities/plf_colony.h"

class BuffDesigner;
class CharacterEffect;
class Randomizer;

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

class EffectDesigner
	: public Systemus<EffectDesigner, Randomizer>
{
public:
	using Super::Super;

	CharacterEffect& DesignCharacterEffect(SEffectRequirements aRequirements);

private:
	std::unique_ptr<CharacterEffect> DecideEffect(const SEffectDesign& aExistingDesign);
	std::unique_ptr<CharacterEffect> DecidePositiveEffect(const SEffectDesign& aExistingDesign);
	std::unique_ptr<CharacterEffect> DecideNegativeEffect(const SEffectDesign& aExistingDesign);
	EEffectType DecideEffectType();
	f32 DecideEffectStrength();

	plf::colony<std::unique_ptr<CharacterEffect>> myEffects;
};
