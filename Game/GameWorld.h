#pragma once
#include "Object/Object.h"
#include "System/System.h"

class GameWorld : public System
{
public:
	GameWorld();

	virtual void Tick() noexcept override;
	Object myPlayer;
	Object myEnemy;
};

