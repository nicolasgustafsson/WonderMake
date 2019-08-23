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

#include "Serialization/SerializationSystem.h"

struct STestComponent
	: public SComponent
{
	int			myKalle = 0;
	std::string	myHobbe;
};

namespace meta {
	template <>
	inline auto registerMembers<STestComponent>()
	{
		return members(
			member("kalle", &STestComponent::myKalle),
			member("hobbe", &STestComponent::myHobbe)
		);
	}
}

class STestFunctionality
	: public Functionality<STestFunctionality, STestComponent>
{
public:
	inline STestFunctionality(Object& aOwner)
		: Super(aOwner)
	{}

	inline void SetKalle(const int aValue)
	{
		Get<STestComponent>().myKalle = aValue;
	}

	inline void SetHobbe(const std::string& aValue)
	{
		Get<STestComponent>().myHobbe = aValue;
	}
};

GameWorld::GameWorld()
{
	SystemPtr<SerializationSystem> serializationSystem;
	serializationSystem->Register<STestComponent>();

	Object objectA;
	auto& test = objectA.Add<STestFunctionality>();

	test.SetKalle(1234);
	test.SetHobbe("asdf");

	const auto testJson = serializationSystem->Serialize(test);
	const std::string data = testJson.dump();

	STestComponent* testB = nullptr;

	//serializationSystem->Deserialize(testJson, object, testB);

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

