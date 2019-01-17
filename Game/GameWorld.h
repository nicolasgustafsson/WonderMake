#pragma once
#include <System/System.h>

class GameWorld : public System
{
public:
	GameWorld();
	~GameWorld();

	virtual void Tick() override;
};

