#pragma once
#include "Object/Object.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"

template<typename TFunctionality>
class FunctionalitySystem;

struct SPlayerDiedMessage;

class LevelDesigner;

class LevelFunctionality;
class TransformFunctionality;
class PlayerControllerFunctionality;
class DefaultMovementFunctionality;
class SpriteRenderingFunctionality;
class CameraFunctionality;

class GameWorld
	: public System<
		Policy::Set<
			PAdd<LevelDesigner, PWrite>,
			PAdd<FunctionalitySystemDelegate<LevelFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<TransformFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<PlayerControllerFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<DefaultMovementFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<SpriteRenderingFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<CameraFunctionality>, PWrite>>>
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

	std::optional<Object> myDeathScreen;
};
