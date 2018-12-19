#pragma once
#include "Threads/Routine.h"

class Program;

class RoutineMain : public Routine
{
public:
	RoutineMain(Program& aProgramReference);

protected:
	virtual void Procedure() override;

private:
	Program& myProgram;
};

