#include "pch.h"
#include "MeleeWeapon.h"
#include "WeaponProperty/WeaponProperty.h"

MeleeWeapon::MeleeWeapon(MeleeWeapon&& aOther) noexcept
{
	myProperties = std::move(aOther.myProperties);
	myBaseWeaponDamage = aOther.myBaseWeaponDamage;
	myBaseWeaponSwingRate = aOther.myBaseWeaponSwingRate;
	myMoveset = std::move(aOther.myMoveset);

	aOther.myBaseWeaponDamage = 0;
	aOther.myBaseWeaponSwingRate = 1.0f;
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

	for (auto&& swing : myMoveset.Moves)
	{
		ImGui::Text("Damage Multiplier: %f", swing.DamageMultipiler);
	}

	ImGui::Separator();
}

