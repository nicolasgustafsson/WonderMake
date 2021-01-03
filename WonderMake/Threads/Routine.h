#pragma once

#include "Message/MessageSubscriber.h"

class Task;

class Routine
{
public:
	Routine();
	virtual ~Routine() = default;

	void Run();

private:
	void RouteMessages();

	void OnTask(const Task& aTask) const;

	MessageSubscriber mySubscriber;
};
