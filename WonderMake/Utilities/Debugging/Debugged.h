#pragma once

#include <string>

#include "Message/MessageSubscriber.h"
#include "Message/MessageTypes.h"

class Debugged
{
public:
	Debugged(const std::string aName) noexcept;
	virtual ~Debugged() noexcept = default;

protected:
	virtual void Debug() = 0;

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber myDebugSubscriber;
	std::string myDebugName;
};

