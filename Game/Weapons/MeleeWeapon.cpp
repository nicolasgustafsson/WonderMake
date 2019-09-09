#include "pch.h"
#include "MeleeWeapon.h"
#include "WeaponProperty/WeaponProperty.h"
#include <Randomizer.h>

MeleeWeapon::MeleeWeapon(MeleeWeapon&& aOther) noexcept
{
	myProperties = std::move(aOther.myProperties);
	myBaseWeaponDamage = aOther.myBaseWeaponDamage;
	myBaseWeaponSwingRate = aOther.myBaseWeaponSwingRate;
}

MeleeWeapon::MeleeWeapon(const SPower aPower)
{
	Generate(aPower);
}

void MeleeWeapon::Inspect()
{
	ImGui::Separator();
	ImGui::Text("Oafish Cudgel of Cloddishery");
	
	ImGui::Text("Damage = %f", myBaseWeaponDamage);
	ImGui::Text("Attack rate = %f", myBaseWeaponSwingRate);

	for (auto& property : myProperties)
	{
		property->Inspect();
	}

	ImGui::Separator();
}

void MeleeWeapon::DrawSwing(const SVector2f aOffset)
{
	for (i32 i = 0.f; i < 19.f; i++)
	{
		const SVector2f start = mySwing.mySwingPath.GetLocationAt((f32)i / 20.f);
		const SVector2f end = mySwing.mySwingPath.GetLocationAt(((f32)i + 1.f) / 20.f);

		WmDrawDebugLine(start + aOffset, end + aOffset, SColor::Yellow, 10.f);
	}
}

void MeleeWeapon::Strengthen(const SPower aPower)
{
	switch (SystemPtr<Randomizer>()->GetRandomNumber(0, 3))
	{
	case 0:
		IncreaseDamage(aPower);
		break;
	case 1:
		IncreaseAttackSpeed(aPower);
		break;
	case 2:
		Strengthen(aPower / 2.f);
		Strengthen(aPower / 2.f);
		break;
	default:
		WmLog("Noo");
	}
}

void MeleeWeapon::IncreaseDamage(const SPower aPower)
{
	myBaseWeaponDamage += aPower;
}

void MeleeWeapon::IncreaseAttackSpeed(const SPower aPower)
{
	myBaseWeaponSwingRate *= 1.0f / (1.0f + (aPower / 100.f));
}

void MeleeWeapon::Weaken(const SPower aPower)
{
	Strengthen(-aPower);
}

SPower MeleeWeapon::GetStrength() const
{
	return SPower{ 1.0f };
}

void MeleeWeapon::Generate(const SPower aPower)
{
	const SPower half = aPower / 2.f;

	IncreaseDamage(half);

	Strengthen(half);
}
