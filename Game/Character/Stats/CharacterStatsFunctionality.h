#pragma once
#include <array>
#include "Typedefs.h"
#include "Components/Component.h"
#include "Functionalities/Functionality.h"

class CharacterFunctionality;

enum class ECharacterStat : u32
{
	MovementSpeed,

	Count
};

struct SStat
{
	float BaseValue;
	float Multiplier = 1.0f;
};

struct SCharacterStatsComponent : public SComponent
{
	CharacterFunctionality* Character;
	std::array<SStat, static_cast<u32>(ECharacterStat::Count)> Stats;
};

class CharacterStatsFunctionality												//Nicos: having 2 functionalities depending on eachother is currently not supported :/
	: public Functionality<CharacterStatsFunctionality, SCharacterStatsComponent/*, CharacterFunctionality*/>
{
public:
	CharacterStatsFunctionality(Object& aOwner);
	f32 GetStatValue(const ECharacterStat aStat) const noexcept;
	 
	void RefreshStats() const;

	void SetBaseValue(const ECharacterStat aStat, const f32 aBaseValue);

	void ApplyMultiplier(const ECharacterStat aStat, const f32 aMultiplier);
	void RemoveMultiplier(const ECharacterStat aStat, const f32 aMultiplier);

private:
	void ApplyStatToCharacter(const ECharacterStat aStat, const CharacterFunctionality& aCharacter) const;

	SStat& GetStat(const ECharacterStat aStat) const noexcept;
};

