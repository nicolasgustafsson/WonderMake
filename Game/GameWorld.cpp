#include "pch.h"
#include "GameWorld.h"

#include "System/System.h"

#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "Designers/LevelDesigner/LevelDesigner.h"
#include "Levels/LevelFunctionality.h"
#include "Camera/CameraFunctionality.h"

REGISTER_SYSTEM(GameWorld);

GameWorld::GameWorld()
	: mySubscriber(ERoutineId::Logic, BindHelper(&GameWorld::OnPlayerDeath, this))
{
	Object player = SetupPlayer();

	LevelFunctionality& level = RestartLevel();

	level.AddDenizen(std::move(player));
}

LevelFunctionality& GameWorld::RestartLevel()
{
	Object newLevel;

	LevelFunctionality& levelFunctionality = Get<FunctionalitySystemDelegate<LevelFunctionality>>().AddFunctionality(newLevel);
	Get<LevelDesigner>().DesignLevel(levelFunctionality);

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

	myPlayerTransform = &Get<FunctionalitySystemDelegate<TransformFunctionality>>().AddFunctionality(player);
	Get<FunctionalitySystemDelegate<PlayerControllerFunctionality>>().AddFunctionality(player);
	Get<FunctionalitySystemDelegate<DefaultMovementFunctionality>>().AddFunctionality(player);

	auto& playerSprite = Get<FunctionalitySystemDelegate<SpriteRenderingFunctionality>>().AddFunctionality(player);
	playerSprite.SetTexture(std::filesystem::current_path() / "Textures/player.png");

	auto& camera = Get<FunctionalitySystemDelegate<CameraFunctionality>>().AddFunctionality(player);
	camera.SetTarget(myPlayerTransform);

	return player;
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	myDeathScreen.emplace();

	auto& sprite = Get<FunctionalitySystemDelegate<SpriteRenderingFunctionality>>().AddFunctionality(*myDeathScreen);
	sprite.SetTexture(std::filesystem::current_path() / "Textures/deadScreen.png");
}


