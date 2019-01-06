// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Program/Engine.h>
#include "GameWorld.h"
#include <Threads/DataThreads.h>
#include <Threads/RoutineIds.h>
#include <Threads/Routine.h>

int main()
{
	GameWorld World;

	Engine::Start([&] 
	{
		DataThreads::Get().GetRoutine(ERoutineId::Logic).lock()->AddProcedure([&] 
		{
			World.Tick();
		});

		WmLog("Game successfully setup!"); 
	});
}
