#pragma once
#include "Functionalities/Functionality.h"

class TransformFunctionality;
class SpriteRenderingFunctionality;
class MeleeWeapon;
class MeleeWeaponFunctionality;

struct SMeleeWeaponUserComponent
	: public SComponent
{
	Object WeaponObject;

	MeleeWeaponFunctionality* myWeapon = nullptr;
};

class MeleeWeaponUserFunctionality
	: public Functionality<MeleeWeaponUserFunctionality, SMeleeWeaponUserComponent, STransformComponent>
{
public:
	MeleeWeaponUserFunctionality(Object& aOwner);

	void Tick();

	void Inspect();

	void SwingWeapon();
	void SetWeapon(MeleeWeapon&& aWeapon);
};