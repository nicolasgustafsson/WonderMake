#pragma once
#include "Utilities/RestrictTypes.h"

#include "Object/Object.h"

struct SLevel
	: public NonCopyable
{
public:
	plf::colony<Object> Objects;
	SVector2f StartPosition;
};
