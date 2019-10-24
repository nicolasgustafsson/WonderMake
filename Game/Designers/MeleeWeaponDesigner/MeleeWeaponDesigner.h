#pragma once
#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"
#include "Weapons/MeleeWeapon.h"


class MeleeWeaponDesigner
	: public System
	//, public Debugged
{
public:
	MeleeWeapon DesignWeapon();

protected:

	std::vector<SSwing> CreateMoveset(const MeleeWeapon& aWeapon) const;

	SSwing DesignSwing(const MeleeWeapon& aWeapon, const bool Mirror) const;

	SSwing DesignFinisher(const MeleeWeapon& aWeapon, const bool Mirror) const;

	//virtual void Debug() override;
};

