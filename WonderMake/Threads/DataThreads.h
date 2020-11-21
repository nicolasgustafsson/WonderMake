#pragma once

#include "Routine.h"

#include "Utilities/Singleton.h"

class Program;

class DataThreads
	: public Singleton<DataThreads>
{
public:
	void Start(Program& aProgramReference, Closure&& aCallback);

	Routine& GetRoutine();

private:
	Routine myRoutine;
};
