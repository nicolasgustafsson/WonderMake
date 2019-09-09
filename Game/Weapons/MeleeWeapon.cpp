#include "pch.h"
#include "MeleeWeapon.h"
#include "WeaponProperty/WeaponProperty.h"

MeleeWeapon::MeleeWeapon(MeleeWeapon&& aOther)
{
	myProperties = std::move(aOther.myProperties);
}

void MeleeWeapon::Inspect()
{
	ImGui::Separator();
	ImGui::Text("Oafish Cudgel of Cloddishery");
	
	for (auto& property : myProperties)
	{
		property->Inspect();
	}

	ImGui::Separator();
}


void MeleeWeapon::Strengthen(const SStrength aStrength)
{

}

void MeleeWeapon::Weaken(const SStrength aStrength)
{

}

SStrength MeleeWeapon::GetStrength() const
{
	return SStrength{ 1.0f };
}

void MeleeWeapon::Generate(SStrength aStrength)
{
}
