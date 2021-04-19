#include "pch.h"
#include "MeleeWeaponUserFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Input/InputSystem.h"
#include "Actions/ActionFunctionality.h"
#include "Weapons/WeaponSwingHitShapeAction.h"
#include "Weapons/MeleeWeapon.h"
#include "Input/InputSystem.h"
#include "Designers/NewDesigners/MeleeWeaponDesigner2/MeleeWeaponDesigner2.h"

REGISTER_COMPONENT(SMeleeWeaponUserComponent);
REGISTER_FUNCTIONALITY(MeleeWeaponUserFunctionality);

void MeleeWeaponUserFunctionality::Inspect()
{
	for (auto&& weapon : Get<SMeleeWeaponUserComponent>().Weapons)
	{
		weapon.Inspect();
	}

	if (ImGui::Button("Randomize"))
	{
		MeleeWeaponDesigner2 weaponDesigner;
		std::vector<MeleeWeapon> weapons;

		weapons.emplace_back(weaponDesigner.Design({}));
		weapons.emplace_back(weaponDesigner.Design({}));

		SetWeapons(std::move(weapons));
	}
}

void MeleeWeaponUserFunctionality::SwingWeapon(u32 aWeaponIndex, SVector2f aDirection)
{
	auto& meleeWeaponUserComponent = Get<SMeleeWeaponUserComponent>();

	if (meleeWeaponUserComponent.Weapons.size() <= aWeaponIndex)
		return;

	auto&& actionFunctionality = Get<ActionFunctionality>();

	auto&& weapon = meleeWeaponUserComponent.Weapons[aWeaponIndex];

	if (weapon.myMoveset.Moves.empty())
		return;

	if (!actionFunctionality.WasLastActionOfType<WeaponSwingHitShapeAction>() || actionFunctionality.TimeSinceLastAction() > 0.5f)
		meleeWeaponUserComponent.CurrentSwingIndex = 0;

	meleeWeaponUserComponent.CurrentSwingIndex %= weapon.myMoveset.Moves.size();

	//EActionResult result = actionFunctionality.StartAction(WeaponSwingAction
	//(Get<CharacterFunctionality>()
	//	, *component.Weapon
	//	, Get<TransformFunctionality2D>()
	//	, weapon.mySwings[component.CurrentSwingIndex]));

	EActionResult result = actionFunctionality.StartAction(WeaponSwingHitShapeAction
	(
		Get<CharacterFunctionality>()
		, aDirection
		, weapon.myMoveset.Moves[meleeWeaponUserComponent.CurrentSwingIndex]
	));

	if (result == EActionResult::Succeeded)
		meleeWeaponUserComponent.CurrentSwingIndex++;
}

void MeleeWeaponUserFunctionality::SetWeapons(std::vector<MeleeWeapon>&& aWeapons)
{
	Get<SMeleeWeaponUserComponent>().Weapons = std::move(aWeapons);
}
