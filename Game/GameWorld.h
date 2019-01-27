#pragma once
#include <System/System.h>
#include <Object/Object.h>

class GameWorld : public System
{
public:
	GameWorld();
	~GameWorld();

	virtual void Tick() override;
	Object myPlayer;
};

