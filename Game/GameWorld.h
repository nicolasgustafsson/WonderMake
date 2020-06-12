#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"
#include "Designers/BuffDesigner/BuffDesigner.h"

struct SPlayerDiedMessage;
class TransformFunctionality;

class GameWorld : public System
{
public:
	GameWorld();

	void RestartLevel();

	Object myPlayer;
	Object myCameraController;
	Object myLevel;

private:
	TransformFunctionality* myPlayerTransform;
	void SetupPlayer();

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;

	SystemPtr<BuffDesigner> myBuffDesigner;

	std::optional<Object> myDeathScreen;
};

