#pragma once
#include "Threads/ThreadProcess.h"

class Program;

class ThreadProcessMain : public ThreadProcess
{
public:
	ThreadProcessMain(Program& aProgramReference);

protected:
	virtual void Procedure() override;

private:
	Program& myProgram;
};

