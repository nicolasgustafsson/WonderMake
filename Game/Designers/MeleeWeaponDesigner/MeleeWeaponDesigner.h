#pragma once
#include "System/System.h"

#include "Weapons/MeleeWeapon.h"

class Randomizer;

//[Nicos]: Very basic atm; when extending, think of how you would design a weapon for a game IRL and code it in here :)
class MeleeWeaponDesigner
	: public System<
		Policy::Set<
			PAdd<Randomizer, PWrite>>>
{
public:
	using Super::Super;

	MeleeWeapon DesignWeapon();

protected:
	std::vector<SAttackMove> CreateMoveset(const MeleeWeapon& aWeapon);

	SAttackMove DesignSwing(const MeleeWeapon& aWeapon, const bool aMirror) const;

	SAttackMove DesignFinisher(const MeleeWeapon& aWeapon, const bool aMirror) const;
};
