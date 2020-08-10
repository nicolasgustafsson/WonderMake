#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"
#include "Designers/BuffDesigner/BuffDesigner.h"
#include "Graphics/ScreenPassRenderObject.h"

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

	void Tick() noexcept override;

private:
	TransformFunctionality* myPlayerTransform = nullptr;
	LevelFunctionality* myCurrentLevelFunctionality = nullptr;

	ScreenPassRenderObject myBackground;

	Object SetupPlayer();

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;

	SystemPtr<BuffDesigner> myBuffDesigner;

	std::optional<Object> myDeathScreen;
};

