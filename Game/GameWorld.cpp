#include "pch.h"
#include "GameWorld.h"

#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "System/System.h"

#include "Designers/LevelDesigner/LevelDesigner.h"

GameWorld::GameWorld()
	:mySubscriber(ERoutineId::Logic, BindHelper(&GameWorld::OnPlayerDeath, this))
{
	EnableTick();
	auto& playerTransform = myPlayer.Add<TransformFunctionality>();
	myPlayer.Add<PlayerControllerFunctionality>();
	myPlayer.Add<DefaultMovementFunctionality>();

	playerTransform.Move({ 0.f, 40.f });
	auto& playerSprite = myPlayer.Add<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	RestartLevel();
}

void GameWorld::RestartLevel()
{
	SystemPtr<LevelDesigner> levelDesigner;

	myLevel = levelDesigner->DesignLevel();
}
void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	myDeathScreen.emplace();

	auto& sprite = myDeathScreen->Add<SpriteRenderingFunctionality>();
	sprite.SetTexture(std::filesystem::current_path() / "Textures/deadScreen.png");
}

