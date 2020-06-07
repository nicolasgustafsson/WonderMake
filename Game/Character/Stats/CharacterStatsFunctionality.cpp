#include "pch.h"
#include "CharacterStatsFunctionality.h"
#include "Character/CharacterFunctionality.h"

CharacterStatsFunctionality::CharacterStatsFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

f32 CharacterStatsFunctionality::GetStatValue(const ECharacterStat aStat) const noexcept
{
	auto stat = Get<SCharacterStatsComponent>().Stats[static_cast<u32>(aStat)];

	return stat.BaseValue * stat.Multiplier;
}

void CharacterStatsFunctionality::RefreshStats() const
{
	if constexpr (Constants::IsDebugging)
	{
		if (Get<SCharacterStatsComponent>().Character == nullptr)
		{
			WmLog(TagError, "Character not set on character stats!");
			return;
		}
	}

	CharacterFunctionality& character = *Get<SCharacterStatsComponent>().Character;

	for (i32 i = 0; i < static_cast<i32>(ECharacterStat::Count); i++)
	{
		ApplyStatToCharacter(static_cast<ECharacterStat>(i), character);
	}
}

void CharacterStatsFunctionality::SetBaseValue(const ECharacterStat aStat, const f32 aBaseValue)
{
	auto& stat = GetStat(aStat);
	stat.BaseValue = aBaseValue;

	RefreshStats();
}

void CharacterStatsFunctionality::ApplyMultiplier(const ECharacterStat aStat, const f32 aMultiplier)
{
	auto& stat = GetStat(aStat);
	stat.Multiplier += (aMultiplier - 1.0f);
	
	RefreshStats();
}

void CharacterStatsFunctionality::RemoveMultiplier(const ECharacterStat aStat, const f32 aMultiplier)
{
	auto& stat = GetStat(aStat);
	stat.Multiplier -= (aMultiplier - 1.0f);

	RefreshStats();
}

void CharacterStatsFunctionality::ApplyStatToCharacter(const ECharacterStat aStat, CharacterFunctionality& aCharacter) const
{
	switch (aStat)
	{
	case ECharacterStat::MovementSpeed:
		aCharacter.Get<DefaultMovementFunctionality>().Get<SDefaultMovementComponent>().MaxMovementSpeed = GetStatValue(aStat);
		break;
	case ECharacterStat::MeleeAttackDamage:
		break;
	default:
		WmLog(TagError, "Stat ", static_cast<i32>(aStat), " has no apply implemented!");
	}
}

SStat& CharacterStatsFunctionality::GetStat(const ECharacterStat aStat) noexcept
{
	return Get<SCharacterStatsComponent>().Stats[static_cast<u32>(aStat)];
}

const SStat& CharacterStatsFunctionality::GetStat(const ECharacterStat aStat) const noexcept
{
	return Get<SCharacterStatsComponent>().Stats[static_cast<u32>(aStat)];
}

void CharacterStatsFunctionality::ApplyStatChange(const SStatChange aStatChange)
{
	ApplyMultiplier(aStatChange.Stat, aStatChange.Multiplier);
}

void CharacterStatsFunctionality::RemoveStatChange(const SStatChange aStatChange)
{
	RemoveMultiplier(aStatChange.Stat, aStatChange.Multiplier);
}

f32 CharacterStatsFunctionality::GetStatMultiplier(const ECharacterStat aStat) const noexcept
{
	return GetStat(aStat).Multiplier;
}

void SStatChange::Inspect() const
{
	const float percentage = Multiplier * 100.f;
	ImGui::Text("%.0f%%", percentage);

	ImGui::SameLine();

	switch (Stat)
	{
	case ECharacterStat::MovementSpeed:
		ImGui::Text("Movement Speed");
		break;
	case ECharacterStat::MeleeAttackDamage:
		ImGui::Text("Melee Attack Damage");
		break;
	}
}
