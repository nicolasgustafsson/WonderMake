#pragma once
#include "System/System.h"
#include "Weapons/MeleeWeapon.h"

//[Nicos]: Very basic atm; when extending, think of how you would design a weapon for a game IRL and code it in here :)
class MeleeWeaponDesigner
	: public System
{
public:
	MeleeWeapon DesignWeapon();

protected:
	std::vector<SSwing> CreateMoveset(const MeleeWeapon& aWeapon) const;

	SSwing DesignSwing(const MeleeWeapon& aWeapon, const bool aMirror) const;

	SSwing DesignFinisher(const MeleeWeapon& aWeapon, const bool aMirror) const;
};

