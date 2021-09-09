#pragma once

#include "Utilities/RestrictTypes.h"

class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	virtual ~_BaseFunctionality() = default;

	virtual void OnNewObject(Object& aObjectAddedTo) { aObjectAddedTo; };

	virtual void Tick() {}
};