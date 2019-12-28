#include "pch.h"
#include "MeleeWeaponDesigner.h"
#include "Randomizer/Randomizer.h"

MeleeWeapon MeleeWeaponDesigner::DesignWeapon()
{
	MeleeWeapon currentWeapon;

	currentWeapon.myBaseWeaponDamage = 100.f;

	currentWeapon.myBaseWeaponSwingRate = 1.5f;

	currentWeapon.mySwings = CreateMoveset(currentWeapon);

	return currentWeapon;
}

std::vector<SSwing> MeleeWeaponDesigner::CreateMoveset(const MeleeWeapon& aWeapon) const
{
	const bool isMirrored = SystemPtr<Randomizer>()->GetRandomBool();

	return {
		DesignSwing(aWeapon, isMirrored),
		DesignSwing(aWeapon, !isMirrored),
		DesignFinisher(aWeapon, isMirrored)
	};
}

SSwing MeleeWeaponDesigner::DesignSwing(const MeleeWeapon& aWeapon, const bool aMirror) const
{
	SSwing swing;

	const bool isFirstSwing = aWeapon.mySwings.empty();
	swing.ChargeTime = isFirstSwing ? 0.4f : 0.f;

	swing.SwingTime = 0.2f;
	swing.BackswingTime = 0.2f;

	swing.IsMirrored = aMirror;
	swing.StepLength = 25.f;

	const f32 horizontalMultiplier = aMirror ? -1.f : 1.f;

	swing.SwingPath = BezierCurve(
		{ -25.f * horizontalMultiplier, 25.f },
		{ 25.f * horizontalMultiplier, 25.f },
		{ -25.f * horizontalMultiplier, -30.f },
		{ 0.f, -100.f });

	return swing;
}

SSwing MeleeWeaponDesigner::DesignFinisher(const MeleeWeapon& aWeapon, const bool aMirror) const
{
	SSwing finisher = DesignSwing(aWeapon, aMirror);

	finisher.IsFinisher = true;

	finisher.ChargeTime = 0.2f;
	finisher.BackswingTime = 0.6f;
	finisher.StepLength = 100.f;

	return finisher;
}