#pragma once
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"
#include "LogMessageTypes.h"

class ConsoleLogger
{
public:
	ConsoleLogger();
	~ConsoleLogger();

private:
	void OnLogMessage(const SLogMessage& aMessage);
	MessageSubscriber mySubscriber;
};

