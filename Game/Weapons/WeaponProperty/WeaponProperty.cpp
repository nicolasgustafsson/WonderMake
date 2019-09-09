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

void WeaponSwingProperty::DrawSwing(const SVector2f aOffset)
{
	for (i32 i = 0.f; i < 19.f; i++)
	{
		const SVector2f start = mySwing.mySwingPath.GetLocationAt((f32)i / 20.f);
		const SVector2f end = mySwing.mySwingPath.GetLocationAt(((f32)i + 1.f) / 20.f);

		WmDrawDebugLine(start + aOffset, end + aOffset, SColor::Yellow, 10.f);
	}
}
