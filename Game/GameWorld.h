#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Levels/Level.h"
#include "Message/MessageSubscriber.h"

struct SPlayerDiedMessage;
class TransformFunctionality;

class GameWorld : public System
{
public:
	GameWorld();

	void RestartLevel();

	Object myPlayer;
	Object myCameraController;
	SLevel myLevel;

private:
	TransformFunctionality* myPlayerTransform;
	void SetupPlayer();

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;

	std::optional<Object> myDeathScreen;
};

