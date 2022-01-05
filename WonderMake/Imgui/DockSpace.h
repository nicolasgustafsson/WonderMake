#pragma once

#include "Message/MessageSubscriber.h"

#include "WonderMakeEngine/System.h"

class DockSpace
	: public System<>
{
public:
	DockSpace();

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber mySubscriber;
};

