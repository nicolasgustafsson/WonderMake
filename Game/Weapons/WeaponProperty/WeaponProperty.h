#pragma once
class WeaponProperty
{
public:
	virtual ~WeaponProperty() = default;

	virtual void Inspect();

	f32 myPotential = 0.f;
};

struct SSwing
{
	f32 mySwingDamage = 10.f;
	f32 mySwingTime = 1.0f;
};

class WeaponSwingProperty
	: public WeaponProperty
{
public:
	virtual void Inspect() override;

	SSwing mySwing;
};