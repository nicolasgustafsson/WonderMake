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

class TestSystem : public System
{
public:
	void Test()
	{
		WmLog("Wow!");
	}

private:
	virtual void Update() override
	{
		WmLog("Hello there!");
	}
};

int main()
{
	GameWorld World;

	SystemContainer::Get().GetSystem<TestSystem>().Test();
	Engine::Start([&] 
	{
		DataThreads::Get().GetRoutine(ERoutineId::Logic).lock()->SetProcedure([&] 
		{
			World.Tick();
		});

		WmLog("Game successfully setup!"); 
	});
}
