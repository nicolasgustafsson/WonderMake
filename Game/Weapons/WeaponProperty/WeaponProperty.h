#pragma once

//Nicos: This is the base class for all unique properties of a weapon: things such as stat changes and special effects
//Nicos: We have none atm :(
class WeaponProperty
{
public:
	virtual ~WeaponProperty() = default;

	virtual void Inspect();
};