#include "pch.h"
#include "MeleeWeaponDesigner2.h"
#include "MeleeWeaponOperations/MeleeWeaponOperations.h"

MeleeWeaponDesigner2::MeleeWeaponDesigner2()
{
	AddOperation<GenerateMoveset>();
	AddOperation<SmashTogetherWeapon>();
}
