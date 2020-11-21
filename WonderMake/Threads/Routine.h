#pragma once

#include "Message/MessageSubscriber.h"

class Task;

class Routine
{
public:
	Routine();
	virtual ~Routine() = default;

	void Run();

	void AddProcedure(Closure aClosure);

protected:
	virtual void Procedure();
	virtual void PreMessageRouting() {};

private:
	void RouteMessages();

	void OnTask(const Task& aTask) const;

	MessageSubscriber mySubscriber;
	plf::colony<Closure> myProcedures;
	plf::colony<Closure> myProceduresToAdd;
};
