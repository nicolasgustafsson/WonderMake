#pragma once
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/MeleeWeapon.h"
#include "Weapons/WeaponSwingAction.h"

struct SMeleeWeaponComponent
	: public SComponent
{
	std::optional<MeleeWeapon> Weapon;
};

class MeleeWeaponFunctionality
	: public Functionality<
		MeleeWeaponFunctionality,
		Policy::Set<
			Policy::Add<SpriteRenderingFunctionality, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<SMeleeWeaponComponent, Policy::EPermission::Write>>>
{
public:
	MeleeWeaponFunctionality(Object& aObject);

	void SetWeapon(MeleeWeapon&& aWeapon);

	TransformFunctionality& GetTransform() { return Get<TransformFunctionality>(); };
	SpriteRenderingFunctionality& GetSprite() { return Get<SpriteRenderingFunctionality>(); };
	MeleeWeapon& GetWeapon();

	void Inspect();
};

