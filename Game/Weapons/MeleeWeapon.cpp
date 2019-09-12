#include "pch.h"
#include "MeleeWeapon.h"
#include "WeaponProperty/WeaponProperty.h"
#include "Randomizer/Randomizer.h"

MeleeWeapon::MeleeWeapon(MeleeWeapon&& aOther) noexcept
{
	myProperties = std::move(aOther.myProperties);
	myBaseWeaponDamage = aOther.myBaseWeaponDamage;
	myBaseWeaponSwingRate = aOther.myBaseWeaponSwingRate;
	mySwing = aOther.mySwing;

	aOther.myBaseWeaponDamage = 0;
	aOther.myBaseWeaponSwingRate = 1.0f;
	aOther.mySwing = SSwing();
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
	const f32 currentPower = GetPower();
	ImGui::Text("Current estimated power = %f", currentPower);

	static f32 Power = 20.f;
	
	if (ImGui::Button("Strengthen"))
	{
		Strengthen(Power);
	}

	ImGui::SameLine();

	if (ImGui::Button("Weaken"))
	{
		Weaken(Power);
	}

	ImGui::SliderFloat("Power to use when Strengthening/Weakening", &Power, 0.f, 1000.f, "%.3f", 2.0f);

	ImGui::Separator();
}

void MeleeWeapon::DrawSwing(const SVector2f aOffset)
{
	for (f32 i = 0.f; i < 19.f; i++)
	{
		const SVector2f start = mySwing.mySwingPath.GetLocationAt((f32)i / 20.f);
		const SVector2f end = mySwing.mySwingPath.GetLocationAt(((f32)i + 1.f) / 20.f);

		WmDrawDebugLine(start + aOffset, end + aOffset, SColor::Yellow, 10.f);
	}
}

void MeleeWeapon::Strengthen(const SPower aPower)
{
	switch (SystemPtr<Randomizer>()->GetRandomNumber(0, 6))
	{
	case 0:
	case 1:
		IncreaseDamage(aPower);
		break;
	case 2:
	case 3:
		IncreaseAttackSpeed(aPower);
		break;
	case 4:
		Strengthen(aPower / 2.f);
		Strengthen(aPower / 2.f);
		break;
	case 5:
		Weaken(aPower);
		Strengthen(aPower);
		Strengthen(aPower);
		break;
	default:
		WmLog("Should not be reached; MeleeWeapon::Strengthen");
	}
}

void MeleeWeapon::IncreaseDamage(const SPower aPower)
{
	myBaseWeaponDamage += aPower / myBaseWeaponSwingRate;

	if (myBaseWeaponDamage < 1.0f)
	{
		myBaseWeaponDamage = 1.f;
	}
}

void MeleeWeapon::IncreaseAttackSpeed(const SPower aPower)
{
	const f32 baseDamage = 50.f;
	myBaseWeaponSwingRate += (aPower / baseDamage) / (myBaseWeaponDamage / baseDamage);

	const f32 MinimumSwingRate = 0.15f;
	if (myBaseWeaponSwingRate < MinimumSwingRate)
		myBaseWeaponSwingRate = MinimumSwingRate;
}

SPower MeleeWeapon::GetPower() const
{
	return myBaseWeaponDamage * myBaseWeaponSwingRate;
}

void MeleeWeapon::Generate(const SPower aPower)
{
	const SPower half = aPower / 2.f;

	IncreaseDamage(half);

	Strengthen(half);
}
