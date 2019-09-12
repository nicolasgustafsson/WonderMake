#pragma once
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/MeleeWeapon.h"

struct SActiveSwing
{
	bool IsActive = false;
	f32 Progress = 0.f;
};

struct SMeleeWeaponComponent
	: public SComponent
{
	TransformFunctionality* ParentTransform;
	std::optional<MeleeWeapon> Weapon;
	SActiveSwing CurrentSwing;
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

	void Swing();
	void SetWeapon(MeleeWeapon&& aWeapon);
	void SetParent(TransformFunctionality* aParentTransform);
	void Tick();

	void Inspect();
private:

	MeleeWeapon& GetWeapon();
	void StopSwing();
};

