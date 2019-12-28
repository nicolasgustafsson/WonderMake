#pragma once
#include "Utilities/RestrictTypes.h"

#include <Object/Object.h>

struct Level
	: public NonCopyable
{
public:
	plf::colony<Object> Enemies;
	Object Portal;

};
