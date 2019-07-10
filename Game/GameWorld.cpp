#include "pch.h"
#include "GameWorld.h"


#include "System/System.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include <Movement/DefaultMovementFunctionality.h>

GameWorld::GameWorld()
{
	EnableTick();
	myPlayer.AddFunctionality<PlayerControllerFunctionality>();
	myPlayer.AddFunctionality<DefaultMovementFunctionality>();
	auto& Sprite = myPlayer.AddFunctionality<SpriteRenderingFunctionality>();
	Sprite.SetTexture(std::filesystem::current_path() / "Textures/tile.png");
	
}

GameWorld::~GameWorld()
{
}

void GameWorld::Tick()
{
}

