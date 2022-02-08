#pragma once

#include "wondermake-utility/RestrictTypes.h"

class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	virtual ~_BaseFunctionality() = default;

	virtual void Tick() {}
};