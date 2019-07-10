#pragma once
#include "Object/Object.h"
#include "System/System.h"

class GameWorld : public System
{
public:
	GameWorld();
	~GameWorld();

	virtual void Tick() override;
	Object myPlayer;
	Object myEnemy;
};

