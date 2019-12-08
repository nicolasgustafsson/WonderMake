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

	playerTransform.Move({ 0.f, 40.f });
	auto& playerSprite = myPlayer.Add<SpriteRenderingFunctionality>();
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& enemyTarget = myEnemy.Add<TargetFunctionality>();
	auto& enemyController = myEnemy.Add<EnemyControllerFunctionality>();
	myEnemy.Add<DefaultMovementFunctionality>();
	auto& enemyCollision = myEnemy.Add<CollisionFunctionality>();
	enemyCollision.AddSphereCollider(enemyController, SVector2f::Zero(), 10.f);
	auto& enemySprite = myEnemy.Add<SpriteRenderingFunctionality>();
	enemySprite.SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
}

void GameWorld::Tick() noexcept
{
}

