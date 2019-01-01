#pragma once

#include "Threads/RoutineIds.h"

#include "Message/MessageSubscriber.h"

class Task;

class Routine
{
public:
	Routine(const ERoutineId aRoutineId);
	virtual ~Routine() = default;

	void Run();

protected:
	virtual void Procedure();

private:
	void RouteMessages();

	void OnTask(const Task& aTask) const;

	const ERoutineId myRoutineId;
	MessageSubscriber mySubscriber;
};
