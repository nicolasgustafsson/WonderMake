#pragma once
#include <array>
#include "Typedefs.h"
#include "Components/Component.h"
#include "Functionalities/Functionality.h"

class CharacterFunctionality;

enum class ECharacterStat : u32
{
	MovementSpeed,
	MeleeAttackDamage,

	Count
};

struct SStat
{
	f32 BaseValue = 0.f;
	f32 Multiplier = 1.0f;
};

struct SStatChange
{
	ECharacterStat Stat;
	f32 Multiplier;

	void Inspect() const;
};

struct SCharacterStatsComponent : public SComponent
{
	CharacterFunctionality* Character;
	std::array<SStat, static_cast<u32>(ECharacterStat::Count)> Stats;
};

class CharacterStatsFunctionality												//Nicos: having 2 functionalities depending on eachother is currently not supported :/
	: public Functionality<
		CharacterStatsFunctionality,
		Policy::Set<
			Policy::Add<SCharacterStatsComponent, Policy::EPermission::Write>>>/*, CharacterFunctionality*/
{
public:
	CharacterStatsFunctionality(Object& aOwner, Dependencies&& aDependencies);
	f32 GetStatValue(const ECharacterStat aStat) const noexcept;
	f32 GetStatMultiplier(const ECharacterStat aStat) const noexcept;
	 
	void RefreshStats() const;

	void SetBaseValue(const ECharacterStat aStat, const f32 aBaseValue);

	void ApplyStatChange(const SStatChange aStatChange);
	void RemoveStatChange(const SStatChange aStatChange);
	void ApplyMultiplier(const ECharacterStat aStat, const f32 aMultiplier);
	void RemoveMultiplier(const ECharacterStat aStat, const f32 aMultiplier);

private:
	void ApplyStatToCharacter(const ECharacterStat aStat, CharacterFunctionality& aCharacter) const;

	SStat& GetStat(const ECharacterStat aStat) noexcept;
	const SStat& GetStat(const ECharacterStat aStat) const noexcept;
};
