#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Object/Object.h"
#include "System/System.h"
#include "Message/MessageSubscriber.h"
#include "Designers/BuffDesigner/BuffDesigner.h"
#include "Graphics/ScreenPassRenderObject.h"

struct SPlayerDiedMessage;

class LevelDesigner;
class ObjectContainerFunctionality;
class ScheduleSystem;

class GameWorld
	: public System<
		Policy::Set<
			PAdd<LevelDesigner, PWrite>,
			PAdd<ScheduleSystem, PWrite>>>
{
public:
	GameWorld();

	void RestartLevel(const bool aAddPlayer = false);

	Object myCameraController;
	Object myLevel;

	void Tick();

private:
	TransformFunctionality2D* myPlayerTransform = nullptr;
	ObjectContainerFunctionality* myCurrentLevelFunctionality = nullptr;

	ScreenPassRenderObject myBackground;

	void SetupPlayer(ObjectContainerFunctionality& aLevelFunctionality);

	void SetLevel(Object aLevel, ObjectContainerFunctionality& aLevelFunctionality, const bool aAddPlayer);

	void OnPlayerDeath(const SPlayerDiedMessage&);
	MessageSubscriber mySubscriber;

	std::optional<Object> myDeathScreen;
};
