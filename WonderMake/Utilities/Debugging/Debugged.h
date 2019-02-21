#pragma once
#include "Utilities/RestrictTypes.h"
#include "Message/MessageSubscriber.h"

class Debugged
{
public:
	Debugged();
	virtual ~Debugged() = default;

protected:
	virtual void Debug() = 0;

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber myDebugSubscriber;
};

