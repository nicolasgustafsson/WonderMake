#include "pch.h"
#include "MeleeWeaponDesigner.h"
#include "Randomizer/Randomizer.h"

REGISTER_SYSTEM(MeleeWeaponDesigner);

MeleeWeapon MeleeWeaponDesigner::DesignWeapon()
{
	MeleeWeapon currentWeapon;

	currentWeapon.myBaseWeaponDamage = 100.f;

	currentWeapon.myBaseWeaponSwingRate = 1.5f;

	currentWeapon.myMoveset.Moves = CreateMoveset(currentWeapon);

	return currentWeapon;
}

std::vector<SAttackMove> MeleeWeaponDesigner::CreateMoveset(const MeleeWeapon& aWeapon)
{
	const bool isMirrored = Get<Randomizer>().GetRandomBool();

	return {
		DesignSwing(aWeapon, isMirrored),
		DesignSwing(aWeapon, !isMirrored),
		DesignFinisher(aWeapon, isMirrored)
	};
}

SAttackMove MeleeWeaponDesigner::DesignSwing(const MeleeWeapon& aWeapon, const bool aMirror) const
{
	SAttackMove swing;

	const bool isFirstSwing = aWeapon.myMoveset.Moves.empty();
	swing.ChargeTime = isFirstSwing ? 0.1f : 0.f;

	swing.SwingTime = 0.2f;

	//swing.IsMirrored = aMirror;
	swing.StepLength = 25.f;

	const f32 sidewayMultiplier = aMirror ? -1.f : 1.f;

	swing.AttackPath = BezierCurve(
		{ 50.f * sidewayMultiplier, -25.f },
		{ -50.f * sidewayMultiplier, -25.f },
		{ 50.f * sidewayMultiplier, 30.f },
		{ 0.f, 100.f });

	return swing;
}

SAttackMove MeleeWeaponDesigner::DesignFinisher(const MeleeWeapon& aWeapon, const bool aMirror) const
{
	SAttackMove finisher = DesignSwing(aWeapon, aMirror);

	//finisher.IsFinisher = true;

	finisher.ChargeTime = 0.2f;
	finisher.StepLength = 100.f;

	return finisher;
}
