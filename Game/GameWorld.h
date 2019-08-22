#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Generation/Weapon/MeleeWeaponGenerator.h"

class GameWorld : public System
{
public:
	GameWorld();
	~GameWorld();

	virtual void Tick() noexcept override;
	Object myPlayer;
	Object myEnemy;
	SystemPtr<MeleeWeaponGenerator> myGenerator;
};

