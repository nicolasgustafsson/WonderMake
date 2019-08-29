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
	STransformComponent* ParentTransform;
	std::optional<MeleeWeapon> Weapon;
	SActiveSwing CurrentSwing;
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
	f32 myRotation = 0;
private:
	void StopSwing();
};

