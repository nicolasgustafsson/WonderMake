#include "stdafx.h"
#include "RoutineMain.h"

#include "Program/Program.h"

#include <iostream>

RoutineMain::RoutineMain(Program& aProgramReference)
	: Routine(ERoutineId::Logic)
	, myProgram(aProgramReference)
{
}

void RoutineMain::Procedure()
{
	myProgram.Update();
	Routine::Procedure();
}
