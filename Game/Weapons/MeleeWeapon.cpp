#include "pch.h"
#include "MeleeWeapon.h"
#include "WeaponProperty/WeaponProperty.h"

MeleeWeapon::MeleeWeapon(const f32 aPotential)
	: myPotential(aPotential)
{

}

MeleeWeapon::MeleeWeapon(MeleeWeapon&& aOther)
{
	myPotential = aOther.myPotential;
	myProperties = std::move(aOther.myProperties);
}

MeleeWeapon::~MeleeWeapon()
{
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
