#pragma once
#include <mutex>
#include "Message/MessageSubscriber.h"

class DockSpace
{
public:
	DockSpace();
	~DockSpace();

private:
	MessageSubscriber mySubscriber;
	void Debug(const SDebugMessage&);
};

