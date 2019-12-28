#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Levels/Level.h"

class GameWorld : public System
{
public:
	GameWorld();

	virtual void Tick() noexcept override;
	Object myPlayer;
	Level myLevel;
};

