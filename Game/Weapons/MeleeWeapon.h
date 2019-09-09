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
	MeleeWeapon(const SPower aPower);
	MeleeWeapon(MeleeWeapon&& aOther) noexcept;

	void Inspect();
	void DrawSwing(const SVector2f aOffset);

	virtual SPower GetStrength() const override;

	SSwing mySwing;
	
	f32 myBaseWeaponDamage = 0.f;
	f32 myBaseWeaponSwingRate = 1.f;
private:
	virtual void Strengthen(const SPower aPower) override;
	void IncreaseDamage(const SPower aPower);
	void IncreaseAttackSpeed(const SPower aPower);
	virtual void Weaken(const SPower aPower) override;

	std::vector<std::unique_ptr<WeaponProperty>> myProperties;

	virtual void Generate(const SPower aPower) override;
};

