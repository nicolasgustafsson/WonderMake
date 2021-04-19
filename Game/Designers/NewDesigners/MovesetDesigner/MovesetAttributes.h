#pragma once
#include "../Attribute/Attribute.h"
#include "Typedefs.h"
#include "Weapons/WeaponSwing.h"
#include "SwingDesigner/SwingAttributes/SwingAttributes.h"

struct SAmountOfMoves : public SAttribute
{
	u32 TotalMoves = 0;
};

enum class EMovesetType
{
	Melee,
	Ranged
};

struct SMovesetType : public SAttribute
{
	EMovesetType Type;
};

struct SMoveTypes : public SAttribute
{
	std::vector<ESwingType> Types;
};

enum class EMeleeMovesetType
{
	ManyWeakAttacks,
	NormalCombo,       
	WeakComboWithStrongFinisher,
	SlowDeadlySwings
};

struct SMeleeMovesetTypeAttribute : public SAttribute
{
	EMeleeMovesetType Moveset;
};