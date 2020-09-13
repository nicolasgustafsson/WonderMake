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
			Policy::Add<LevelDesigner, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<LevelFunctionality>, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<TransformFunctionality>, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<PlayerControllerFunctionality>, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<DefaultMovementFunctionality>, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<SpriteRenderingFunctionality>, Policy::EPermission::Write>,
			Policy::Add<FunctionalitySystemDelegate<CameraFunctionality>, Policy::EPermission::Write>>>
{
public:
	GameWorld(Dependencies&& aDependencies);

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
