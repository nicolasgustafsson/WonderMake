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
	: public Functionality
	< MeleeWeaponFunctionality
	, SpriteRenderingFunctionality
	, TransformFunctionality
	, SMeleeWeaponComponent>
{
public:
	MeleeWeaponFunctionality(Object& aObject);

	void SetWeapon(MeleeWeapon&& aWeapon);

	TransformFunctionality& GetTransform() { return Get<TransformFunctionality>(); };
	SpriteRenderingFunctionality& GetSprite() { return Get<SpriteRenderingFunctionality>(); };
	MeleeWeapon& GetWeapon();

	void Inspect();
};

