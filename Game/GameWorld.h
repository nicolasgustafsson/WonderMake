#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Levels/Level.h"
#include "Message/MessageSubscriber.h"

struct SPlayerDiedMessage;

class GameWorld : public System
{
public:
	GameWorld();

	void RestartLevel();

	Object myPlayer;
	Level myLevel;

private:

	std::optional<Object> myDeathScreen;

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;
};

