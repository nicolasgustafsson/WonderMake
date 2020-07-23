#include "pch.h"
#include "GameWorld.h"

#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "System/System.h"

#include "Designers/LevelDesigner/LevelDesigner.h"
#include "Camera/CameraFunctionality.h"
#include "Levels/LevelFunctionality.h"

GameWorld::GameWorld(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
	, mySubscriber(ERoutineId::Logic, BindHelper(&GameWorld::OnPlayerDeath, this))
{
	Object player = SetupPlayer();

	LevelFunctionality& level = RestartLevel();

	level.AddDenizen(std::move(player));
}

LevelFunctionality& GameWorld::RestartLevel()
{
	SystemPtr<LevelDesigner> levelDesigner;
	
	Object newLevel;

	LevelFunctionality& levelFunctionality = newLevel.Add<LevelFunctionality>();
	levelDesigner->DesignLevel(levelFunctionality);

	if (myCurrentLevelFunctionality)
		myCurrentLevelFunctionality->TransferToNewLevel(levelFunctionality);

	myPlayerTransform->SetPosition(levelFunctionality.GetStartPosition());
	
	myLevel = std::move(newLevel);

	myCurrentLevelFunctionality = &levelFunctionality;

	return levelFunctionality;
}

Object GameWorld::SetupPlayer()
{
	Object player;
	myPlayerTransform = &player.Add<TransformFunctionality>();
	player.Add<PlayerControllerFunctionality>();
	player.Add<DefaultMovementFunctionality>();

	auto& playerSprite = player.Add<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& camera = myCameraController.Add<CameraFunctionality>();
	camera.SetTarget(myPlayerTransform);

	return player;
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	myDeathScreen.emplace();

	auto& sprite = myDeathScreen->Add<SpriteRenderingFunctionality>();
	sprite.SetTexture(std::filesystem::current_path() / "Textures/deadScreen.png");
}


