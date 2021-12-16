#pragma once

#include "Utilities/RestrictTypes.h"

class BaseObject;

class _BaseFunctionality
	: public NonCopyable
{
public:
	virtual ~_BaseFunctionality() = default;

	virtual void OnNewObject(BaseObject& aObjectAddedTo) { aObjectAddedTo; };

	virtual void Tick() {}
};