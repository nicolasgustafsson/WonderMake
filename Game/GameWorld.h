#pragma once
#include "Object/Object.h"

#include "System/System.h"

#include "Utilities/Debugging/Debugged.h"

class GameWorld
	: public System
	, public Debugged
{
public:
	GameWorld();

	virtual void Tick() noexcept override;

private:
	virtual void Debug() override;

	Object myPlayer;
	Object myEnemy;
};

