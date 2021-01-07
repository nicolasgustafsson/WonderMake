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
		.Then([this, aAddPlayer](auto&& aObject, auto&& aFunctionalities)
			{
				auto&& levelFunctionality = std::get<LevelFunctionality&>(aFunctionalities);

				Get<LevelDesigner>().DesignLevel(levelFunctionality);

				if (myCurrentLevelFunctionality)
					myCurrentLevelFunctionality->TransferToNewLevel(levelFunctionality);

				if (myPlayerTransform)
				{
					myPlayerTransform->SetPosition(SVector2f::Zero());
					//myPlayerTransform->SetPosition(levelFunctionality.GetStartPosition());
				}

				myLevel = std::move(aObject);

				myCurrentLevelFunctionality = &levelFunctionality;

				if (!aAddPlayer)
					return;

				SetupPlayer(levelFunctionality);
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
		.Then([this, &aLevelFunctionality](auto&& aObject, auto&& aFunctionalities)
			{
				myPlayerTransform = &std::get<TransformFunctionality2D&>(aFunctionalities);

				std::get<CameraFunctionality&>(aFunctionalities).SetTarget(myPlayerTransform);

				aLevelFunctionality.AddDenizen(std::move(aObject));
			});
}

void GameWorld::OnPlayerDeath(const SPlayerDiedMessage&)
{
	RunJob<CreateObjectJob<SpriteRenderingFunctionality>>()
		.Then([this](auto&& aObject, auto&& aFunctionalities)
			{
				auto& sprite = std::get<SpriteRenderingFunctionality&>(aFunctionalities);

			sprite.SetTexture("Death screen");

				myDeathScreen.emplace(std::move(aObject));
			});
}


