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

	void AddProcedure(Closure aClosure);

protected:
	virtual void Procedure();
	virtual void PreMessageRouting() {};

private:
	void RouteMessages();

	void OnTask(const Task& aTask) const;

	const ERoutineId myRoutineId;
	MessageSubscriber mySubscriber;
	plf::colony<Closure> myProcedures;
};
