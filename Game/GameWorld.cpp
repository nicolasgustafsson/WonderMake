#include "pch.h"
#include "GameWorld.h"

#include "Collision/CollisionFunctionality.h"
#include "Collision/CollisionSystem.h"
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
	auto& playerTransform = myPlayer.AddFunctionality<TransformFunctionality>();
	myPlayer.AddFunctionality<PlayerControllerFunctionality>();
	myPlayer.AddFunctionality<DefaultMovementFunctionality>();
	auto& playerSprite = myPlayer.AddFunctionality<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& enemyTarget = myEnemy.AddFunctionality<TargetFunctionality>();
	enemyTarget.Temp = &playerTransform;
	myEnemy.AddFunctionality<EnemyControllerFunctionality>();
	myEnemy.AddFunctionality<DefaultMovementFunctionality>();
	auto& enemyCollision = myEnemy.AddFunctionality<CollisionFunctionality>();
	enemyCollision.AddSphereCollider(SVector2f::Zero(), 75.f);
	auto& enemySprite = myEnemy.AddFunctionality<SpriteRenderingFunctionality>();
	enemySprite.SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
	
}

GameWorld::~GameWorld()
{
}

void GameWorld::Tick() noexcept
{
}

