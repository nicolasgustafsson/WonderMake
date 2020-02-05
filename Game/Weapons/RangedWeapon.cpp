#include "pch.h"
#include "RangedWeapon.h"

void RangedWeapon::Inspect()
{
	ImGui::Separator();
	ImGui::Text("Short Bow of Impish Retort");

	ImGui::Text("Damage = %f", myBaseWeaponDamage);

	ImGui::Separator();
}

