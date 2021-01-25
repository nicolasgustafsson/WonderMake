#pragma once
#include "../Attribute/Attribute.h"
#include "Typedefs.h"

struct SAmountOfMoves : public SAttribute
{
	u32 MovesLeft = 0;
};