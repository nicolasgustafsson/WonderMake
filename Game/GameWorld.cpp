#include "pch.h"
#include "GameWorld.h"

#include "Job/Cargo.h"

#include "System/System.h"

#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

#include "Movement/DefaultMovementFunctionality.h"
#include "Player/PlayerControllerFunctionality.h"
#include "Designers/LevelDesigner/LevelDesigner.h"
#include "Levels/LevelFunctionality.h"
#include "Camera/CameraFunctionality.h"

#include "Object/CreateObjectJob.h"

#include "Scheduling/ScheduleSystem.h"

REGISTER_SYSTEM(GameWorld);

GameWorld::GameWorld()
	: mySubscriber(BindHelper(&GameWorld::OnPlayerDeath, this)), myBackground(std::filesystem::current_path() / "Shaders/Fragment/Background.frag")
{
	myBackground.SetRenderLayer("Background");
	myBackground.SetRenderOrder(-9999);
	myBackground.SetProperty("MainColor", SColor::RaisinBlack());
	myBackground.SetProperty("DetailColor", SColor::Seashell());

	RestartLevel(true);

	Get<ScheduleSystem>().ScheduleRepeating<>([this]() { Tick(); });
}

void GameWorld::RestartLevel(const bool aAddPlayer)
{
	RunJob<CreateObjectJob<LevelFunctionality>>()
		.Then(WrapCargo(*this), [aAddPlayer](Cargo<GameWorld>&& aCargo, Object&& aObject, Cargo<LevelFunctionality>&& aFunctionalities)
			{
				aCargo.Get<GameWorld>().SetLevel(std::move(aObject), aFunctionalities.Get<LevelFunctionality>(), aAddPlayer);
			});
}

void GameWorld::Tick()
{
	myBackground.SetProperty("MainColor", SColor::RaisinBlack());
	myBackground.SetProperty("DetailColor", SColor::Seashell());
	myBackground.Render();
}

void GameWorld::SetupPlayer(LevelFunctionality& aLevelFunctionality)
{
	RunJob<CreateObjectJob<TransformFunctionality2D, PlayerControllerFunctionality, CameraFunctionality>>()
		.Then(WrapCargo(*this, aLevelFunctionality), [](auto&& aCargo, auto&& aObject, auto&& aFunctionalities)
			{
				auto&& gameWorld = aCargo.Get<GameWorld>();
				auto&& levelFunctionality = aCargo.Get<LevelFunctionality>();

				levelFunctionality.AddDenizen(std::move(aObject));
				gameWorld.myPlayerTransform = &aFunctionalities.Get<TransformFunctionality2D>();

				aFunctionalities.Get<CameraFunctionality>().SetTarget(gameWorld.myPlayerTransform);
			});
}

void GameWorld::SetLevel(Object aLevel, LevelFunctionality& aLevelFunctionality, const bool aAddPlayer)
{
	Get<LevelDesigner>().DesignLevel(aLevelFunctionality);

	if (myCurrentLevelFunctionality)
		myCurrentLevelFunctionality->TransferToNewLevel(aLevelFunctionality);

	if (myPlayerTransform)
	{
		myPlayerTransform->SetPosition(SVector2f::Zero());
		//myPlayerTransform->SetPosition(levelFunctionality.GetStartPosition());
	}

	myLevel = std::move(aLevel);

	myCurrentLevelFunctionality = &aLevelFunctionality;

	if (!aAddPlayer)
		return;

	SetupPlayer(aLevelFunctionality);
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	RunJob<CreateObjectJob<SpriteRenderingFunctionality>>()
		.Then(WrapCargo(*this), [](auto&& aCargo, auto&& aObject, auto&& aFunctionalities)
			{
				auto&& gameWorld = aCargo.Get<GameWorld>();
				auto&& sprite = aFunctionalities.Get<SpriteRenderingFunctionality>();

				sprite.SetTexture("Death screen");

				gameWorld.myDeathScreen.emplace(std::move(aObject));
			});
}


