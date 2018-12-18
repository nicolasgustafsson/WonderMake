#include "stdafx.h"
#include "ThreadProcessMain.h"

#include "Program/Program.h"

#include <iostream>

ThreadProcessMain::ThreadProcessMain(Program& aProgramReference)
	: ThreadProcess(EThreadId::Logic)
	, myProgram(aProgramReference)
{
}

void ThreadProcessMain::Procedure()
{
	myProgram.Update();
}
