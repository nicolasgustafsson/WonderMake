#pragma once
#include "../../../Attribute/Attribute.h"
#include "Typedefs.h"
#include "Utilities/BezierCurve.h"
#include "Weapons/WeaponSwing.h"

enum class ESwingType
{
	Stab,
	Slash
};

enum class ESwingThreat
{
	Pathetic,
	Normal,
	Stronk
};

enum class ESwingSpeed
{
	Snail,
	Snake,
	Cheetah
};

struct SSwingInProgressAttribute : public SAttribute
{
	SSwing SwingInProgress;
};


//Boilerplate = creating new classes for every attribute - can we make some template instead or something?

struct SSwingTypeAttribute : public SAttribute
{
	ESwingType Type;
};

struct SSwingThreatAttribute : public SAttribute
{
	ESwingThreat SwingThreat;
};

struct SSwingSpeedAttribute : public SAttribute
{
	ESwingSpeed SwingSpeed;
};
