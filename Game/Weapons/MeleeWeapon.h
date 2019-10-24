#pragma once
#include "Weapons/WeaponProperty/WeaponProperty.h"
#include "Generation/GeneratedObject.h"
#include "Generation/GenerationParameters/Power.h"
#include "Weapons/WeaponSwing.h"

class MeleeWeapon
	: public NonCopyable
	, public GeneratedObject<SPower>
{
public:
	MeleeWeapon();
	MeleeWeapon(const SPower aPower);
	MeleeWeapon(MeleeWeapon&& aOther) noexcept;

	void Inspect();

	[[nodiscard]] virtual SPower GetPower() const override;

	std::vector<SSwing> mySwings;
	
	f32 myBaseWeaponDamage = 0.f;
	f32 myBaseWeaponSwingRate = 1.f;

private:
	virtual void Strengthen(const SPower aPower) override;
	void IncreaseDamage(const SPower aPower);
	void IncreaseAttackSpeed(const SPower aPower);

	std::vector<std::unique_ptr<WeaponProperty>> myProperties;

	virtual void Generate(const SPower aPower) override;
};

