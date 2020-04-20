#include "pch.h"
#include "GameWorld.h"

#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "System/System.h"

#include "Designers/LevelDesigner/LevelDesigner.h"
#include "Camera/CameraFunctionality.h"

GameWorld::GameWorld()
	:mySubscriber(ERoutineId::Logic, BindHelper(&GameWorld::OnPlayerDeath, this))
{
	EnableTick();

	SetupPlayer();

	RestartLevel();
}

void GameWorld::RestartLevel()
{
	SystemPtr<LevelDesigner> levelDesigner;

	myLevel = levelDesigner->DesignLevel();

	myPlayerTransform->SetPosition(myLevel.StartPosition);
}

void GameWorld::SetupPlayer()
{
	myPlayerTransform = &myPlayer.Add<TransformFunctionality>();
	myPlayer.Add<PlayerControllerFunctionality>();
	myPlayer.Add<DefaultMovementFunctionality>();

	//auto& playerSprite = myPlayer.Add<SpriteRenderingFunctionality>();
	//playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& camera = myCameraController.Add<CameraFunctionality>();
	camera.SetTarget(myPlayerTransform);
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	myDeathScreen.emplace();

	auto& sprite = myDeathScreen->Add<SpriteRenderingFunctionality>();
	sprite.SetTexture(std::filesystem::current_path() / "Textures/deadScreen.png");
}


