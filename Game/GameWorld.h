#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Levels/Level.h"

class GameWorld : public System
{
public:
	GameWorld();

	void RestartLevel();

	Object myPlayer;
	Level myLevel;
};

