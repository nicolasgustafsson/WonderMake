#pragma once
#include "../WeaponSwing.h"
class WeaponProperty
{
public:
	virtual ~WeaponProperty() = default;

	virtual void Inspect();
};

class WeaponSwingProperty
	: public WeaponProperty
{
public:
	virtual void Inspect() override;

	void DrawSwing(const SVector2f aOffset);
	SSwing mySwing;
};