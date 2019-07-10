#include "pch.h"
#include "GameWorld.h"

#include "Enemy/EnemyControllerFunctionality.h"
#include "Enemy/TargetFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "System/System.h"

GameWorld::GameWorld()
{
	EnableTick();
	auto& transform = myPlayer.AddFunctionality<TransformFunctionality>();
	myPlayer.AddFunctionality<PlayerControllerFunctionality>();
	myPlayer.AddFunctionality<DefaultMovementFunctionality>();
	auto& playerSprite = myPlayer.AddFunctionality<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/tile.png");

	auto& target = myEnemy.AddFunctionality<TargetFunctionality>();
	target.Temp = &transform;
	myEnemy.AddFunctionality<EnemyControllerFunctionality>();
	myEnemy.AddFunctionality<DefaultMovementFunctionality>();
	auto& enemySprite = myEnemy.AddFunctionality<SpriteRenderingFunctionality>();
	enemySprite.SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
	
}

GameWorld::~GameWorld()
{
}

void GameWorld::Tick()
{
}

