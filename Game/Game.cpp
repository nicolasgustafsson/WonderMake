// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Program/Engine.h>
#include "GameWorld.h"
#include <Threads/DataThreads.h>
#include <Threads/RoutineIds.h>
#include <Threads/Routine.h>
#include <System/System.h>
#include <System/SystemContainer.h>

int main()
{
	Engine::Start([&] 
	{
		SystemContainer::Get().CreateSystem<GameWorld>();

		WmLog("Game successfully setup!"); 
	});
}
