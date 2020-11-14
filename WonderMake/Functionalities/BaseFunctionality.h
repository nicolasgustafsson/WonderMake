#pragma once

#include "Utilities/RestrictTypes.h"

class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	virtual ~_BaseFunctionality() = default;

	void Tick() noexcept {}
};