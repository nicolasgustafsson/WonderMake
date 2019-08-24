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
	auto& playerTransform = myPlayer.Add<TransformFunctionality>();
	myPlayer.Add<PlayerControllerFunctionality>();
	myPlayer.Add<DefaultMovementFunctionality>();
	auto& playerSprite = myPlayer.Add<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& enemyTarget = myEnemy.Add<TargetFunctionality>();
	enemyTarget.Temp = &playerTransform;
	myEnemy.Add<EnemyControllerFunctionality>();
	myEnemy.Add<DefaultMovementFunctionality>();
	auto& enemyCollision = myEnemy.Add<CollisionFunctionality>();
	enemyCollision.AddSphereCollider(SVector2f::Zero(), 75.f);
	auto& enemySprite = myEnemy.Add<SpriteRenderingFunctionality>();
	enemySprite.SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
	
}

GameWorld::~GameWorld()
{
}

void GameWorld::Tick() noexcept
{
}

