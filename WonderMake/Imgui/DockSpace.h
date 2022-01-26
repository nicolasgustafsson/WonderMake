#pragma once

#include "Message/MessageSubscriber.h"

#include "WonderMakeBase/System.h"

class DockSpace
	: public System<>
{
public:
	DockSpace();

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber mySubscriber;
};

