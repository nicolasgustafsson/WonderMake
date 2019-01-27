#include "pch.h"
#include "GameWorld.h"


#include "System/System.h"
#include <Object/Functionalities/SpriteRenderingFunctionality.h>

GameWorld::GameWorld()
{
	EnableTick();
	auto* Sprite = myPlayer.AddFunctionality<SpriteRenderingFunctionality>();
	Sprite->SetTexture(std::filesystem::current_path() / "Textures/tile.png");
}

GameWorld::~GameWorld()
{
}

void GameWorld::Tick()
{
}

