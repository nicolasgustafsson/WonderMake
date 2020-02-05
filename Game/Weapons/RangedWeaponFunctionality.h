#pragma once
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Weapons/RangedWeapon.h"

struct SRangedWeaponComponent
	: public SComponent
{
	std::optional<RangedWeapon> Weapon;
};

class RangedWeaponFunctionality
	: public Functionality<
		RangedWeaponFunctionality,
		SpriteRenderingFunctionality,
		TransformFunctionality,
		SRangedWeaponComponent>
{
public:
	RangedWeaponFunctionality(Object& aObject);

	void SetWeapon(RangedWeapon&& aWeapon);

	TransformFunctionality& GetTransform() { return Get<TransformFunctionality>(); };
	SpriteRenderingFunctionality& GetSprite() { return Get<SpriteRenderingFunctionality>(); };
	RangedWeapon& GetWeapon();

	void Inspect();
};
