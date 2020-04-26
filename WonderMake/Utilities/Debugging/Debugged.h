#pragma once
#include "Message/MessageSubscriber.h"

class Debugged
{
public:
	Debugged(const std::string aName) noexcept;
	virtual ~Debugged() = default;

protected:
	virtual void Debug() = 0;

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber myDebugSubscriber;
	std::string DebugName;
};

