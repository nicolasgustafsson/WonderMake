#include "pch.h"
#include "MeleeWeaponDesigner.h"
#include "Randomizer/Randomizer.h"

MeleeWeapon MeleeWeaponDesigner::DesignWeapon()
{
	MeleeWeapon currentWeapon = MeleeWeapon();

	currentWeapon.myBaseWeaponDamage = 100.f;

	currentWeapon.myBaseWeaponSwingRate = 1.5f;

	currentWeapon.mySwings = CreateMoveset(currentWeapon);

	return currentWeapon;
}

std::vector<SSwing> MeleeWeaponDesigner::CreateMoveset(const MeleeWeapon& aWeapon) const
{
	std::vector<SSwing> moveset;
	bool IsMirrored = SystemPtr<Randomizer>()->GetRandomBool();

	moveset.push_back(DesignSwing(aWeapon, IsMirrored));

	IsMirrored = !IsMirrored;

	moveset.push_back(DesignSwing(aWeapon, IsMirrored));

	IsMirrored = !IsMirrored;

	moveset.push_back(DesignFinisher(aWeapon, IsMirrored));

	return moveset; 
}

SSwing MeleeWeaponDesigner::DesignSwing(const MeleeWeapon& aWeapon, const bool Mirror) const
{
	SSwing swing;

	const bool isFirstSwing = aWeapon.mySwings.size() == 0;
	if (isFirstSwing)
		swing.ChargeTime = 0.4f;
	else
		swing.ChargeTime = 0.1f;

	swing.SwingTime = 0.2f;
	swing.BackswingTime = 0.2f;

	swing.IsMirrored = Mirror;
	swing.StepLength = 25.f;

	const f32 HorizontalMultiplier = Mirror ? -1.f : 1.f;

	swing.SwingPath = BezierCurve(
		{ -25.f * HorizontalMultiplier, 25.f },
		{ 25.f * HorizontalMultiplier, 25.f },
		{ -25.f * HorizontalMultiplier, -30.f },
		{ 0.f, -100.f });

	return swing;
}

SSwing MeleeWeaponDesigner::DesignFinisher(const MeleeWeapon& aWeapon, const bool Mirror) const
{
	SSwing finisher = DesignSwing(aWeapon, Mirror);

	finisher.IsFinisher = true;

	finisher.ChargeTime = 0.2f;
	finisher.BackswingTime = 0.6f;
	finisher.StepLength = 100.f;

	return finisher;
}
