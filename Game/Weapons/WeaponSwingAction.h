#pragma once
#include "Actions/Action.h"
#include "Weapons/WeaponSwing.h"

class TransformFunctionality;
class SpriteRenderingFunctionality;
class MeleeWeapon;
class MeleeWeaponFunctionality;

struct SActiveSwing
{
	bool IsActive = false;

	f32 Progress = 0.f;
};

class WeaponSwingAction
	: public Action
{
public:
	WeaponSwingAction(MeleeWeaponFunctionality& aMeleeWeaponFunctionality, TransformFunctionality& aUserTransform);

	virtual void BeginAction() override;

	virtual void Tick() override;

	virtual bool IsCompleted() override;

	virtual void EndAction() override;

private:
	TransformFunctionality& myUserTransform;
	MeleeWeaponFunctionality& myWeaponFunctionality;
	
	f32 myProgress = 0.f;
};

