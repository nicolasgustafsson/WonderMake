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
	: mySubscriber(ERoutineId::Logic, BindHelper(&GameWorld::OnPlayerDeath, this)), myBackground(std::filesystem::current_path() / "Shaders/Fragment/Background.frag")
{
	Object player = SetupPlayer();

	myBackground.SetRenderLayer("Background");
	myBackground.SetRenderOrder(-9999);
	myBackground.SetProperty("MainColor", SColor::RaisinBlack());
	myBackground.SetProperty("DetailColor", SColor::Seashell());

	LevelFunctionality& level = RestartLevel();

	level.AddDenizen(std::move(player));

	EnableTick();
}

LevelFunctionality& GameWorld::RestartLevel()
{
	Object newLevel;

	LevelFunctionality& levelFunctionality = Get<FunctionalitySystemDelegate<LevelFunctionality>>().AddFunctionality(newLevel);
	Get<LevelDesigner>().DesignLevel(levelFunctionality);

	if (myCurrentLevelFunctionality)
		myCurrentLevelFunctionality->TransferToNewLevel(levelFunctionality);

	myPlayerTransform->SetPosition(SVector2f::Zero());
	//myPlayerTransform->SetPosition(levelFunctionality.GetStartPosition());
	
	myLevel = std::move(newLevel);

	myCurrentLevelFunctionality = &levelFunctionality;

	return levelFunctionality;
}

void GameWorld::Tick() noexcept
{
	myBackground.SetProperty("MainColor", SColor::RaisinBlack());
	myBackground.SetProperty("DetailColor", SColor::Seashell());
	myBackground.Render();
}

Object GameWorld::SetupPlayer()
{
	Object player;
	myPlayerTransform = &Get<FunctionalitySystemDelegate<TransformFunctionality>>().AddFunctionality(player);
	Get<FunctionalitySystemDelegate<PlayerControllerFunctionality>>().AddFunctionality(player);
	Get<FunctionalitySystemDelegate<CameraFunctionality>>().AddFunctionality(player).SetTarget(myPlayerTransform);

	return player;
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	myDeathScreen.emplace();

	auto& sprite = Get<FunctionalitySystemDelegate<SpriteRenderingFunctionality>>().AddFunctionality(*myDeathScreen);
	sprite.SetTexture(std::filesystem::current_path() / "Textures/deadScreen.png");
}


