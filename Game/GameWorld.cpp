#include "pch.h"
#include "GameWorld.h"
#include <Threads/DataThreads.h>
#include <Threads/Routine.h>


GameWorld::GameWorld()
{
	DataThreads::Get().GetRoutine(ERoutineId::Logic).lock()->AddProcedure([this] {Tick(); });
}


GameWorld::~GameWorld()
{
}

void GameWorld::Tick()
{
	WmLog("Hello there!");
}
