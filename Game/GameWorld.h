#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"
#include "Designers/BuffDesigner/BuffDesigner.h"

struct SPlayerDiedMessage;
class TransformFunctionality;
class LevelFunctionality;

class GameWorld : public System
{
public:
	GameWorld();

	LevelFunctionality& RestartLevel();

	Object myCameraController;
	Object myLevel;

private:
	TransformFunctionality* myPlayerTransform = nullptr;
	LevelFunctionality* myCurrentLevelFunctionality = nullptr;

	Object SetupPlayer();

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;

	SystemPtr<BuffDesigner> myBuffDesigner;

	std::optional<Object> myDeathScreen;
};

