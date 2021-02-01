#pragma once
#include "../Attribute/Attribute.h"
#include "Typedefs.h"

struct SAmountOfMoves : public SAttribute
{
	u32 TotalMoves = 0;
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