#pragma once

#include "Message/MessageSubscriber.h"

#include "wondermake-base/System.h"

class DockSpace
	: public System<>
{
public:
	DockSpace();

private:
	void OnDebugMessage(const SDebugMessage&);

	MessageSubscriber mySubscriber;
};

