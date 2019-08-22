#include "pch.h"
#include "WeaponProperty.h"

void WeaponProperty::Inspect()
{
	ImGui::Text("Does important work for the community");
}

void WeaponSwingProperty::Inspect()
{
	ImGui::Text("Swing: %f damage in %f seconds", mySwing.mySwingDamage, mySwing.mySwingTime);
}
