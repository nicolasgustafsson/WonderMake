#pragma once

#include "Typedefs.h"

class Action
{
public:
	virtual ~Action() = default;

	virtual void BeginAction() {}

	virtual void Tick(const f32 aDeltaTime) { aDeltaTime; };

	virtual bool IsCompleted() const = 0;

	virtual void EndAction() {}

	virtual void Inspect() {}

	virtual bool CanBeInterrupted() const { return false; }

	virtual bool BlocksMovementInput() const { return true; }
};

