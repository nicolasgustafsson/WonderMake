// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "Game.h"
#include "GameWorld.h"

#include "System/SystemContainer.h"
#include "Utilities/Container/BackendImplementations/ColonyBackend.h"
#include "Utilities/TypeTraits/ContainsType.h"

void Start()
{
	SystemContainer::Get().CreateSystem<GameWorld>();
}
