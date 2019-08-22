#pragma once
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/MeleeWeapon.h"

struct SMeleeWeaponComponent
	: public SComponent
{
	STransformComponent* ParentTransform;
	std::optional<MeleeWeapon> myWeapon;
};

class MeleeWeaponFunctionality
	: public Functionality<MeleeWeaponFunctionality, SpriteRenderingFunctionality, TransformFunctionality, SMeleeWeaponComponent>
{
public:
	MeleeWeaponFunctionality(Object& aObject);

	void Swing();
	void SetWeapon(MeleeWeapon&& aWeapon);
	void SetParent(STransformComponent* aParentTransform);
	void Tick();
};

