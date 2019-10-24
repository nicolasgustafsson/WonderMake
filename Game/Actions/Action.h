#pragma once
class Action
{
public:
	virtual ~Action() = default;

	virtual void BeginAction() {}

	virtual void Tick() {};

	virtual bool IsCompleted() = 0;

	virtual void EndAction() {}

	virtual void Inspect() {}

	virtual bool CanBeInterrupted() { return false; }

	virtual bool BlocksMovementInput() { return true; }
};

