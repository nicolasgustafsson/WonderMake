#pragma once

#include "WonderMakeUtility/RestrictTypes.h"

class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	virtual ~_BaseFunctionality() = default;

	virtual void Tick() {}
};