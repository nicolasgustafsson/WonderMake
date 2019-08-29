#pragma once
#include "BezierCurve.h"
class WeaponProperty
{
public:
	virtual ~WeaponProperty() = default;

	virtual void Inspect();

	f32 myPotential = 0.f;
};

struct SSwing
{
	SSwing();

	f32 mySwingDamage = 10.f;
	f32 mySwingTime =  0.5f;

	BezierCurve mySwingPath;
};

class WeaponSwingProperty
	: public WeaponProperty
{
public:
	virtual void Inspect() override;

	void DrawSwing(const SVector2f aOffset);
	SSwing mySwing;
};