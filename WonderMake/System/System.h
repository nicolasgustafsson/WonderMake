#pragma once
#include "Message/MessageSubscriber.h"

class SystemContainer;

class System 
	: public NonCopyable
{
public:
	virtual ~System() = default;

	virtual void Tick() noexcept {}

protected:
	void EnableTick();

	System() = default;
};

