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
#include "Serialization/SerializationSystem.h"
#include "System/System.h"

inline void ImguiJson(const std::string& aKey, json& aJson)
{
	if (aJson.is_array())
	{
		ImGui::Text(aKey.c_str());
		for (size_t i = 0; i < aJson.size(); ++i)
		{
			std::string indexStr;

			indexStr += i;

			ImguiJson(indexStr, aJson[i]);
		}
	}
	else if (aJson.is_object())
	{
		ImGui::Text(aKey.c_str());
		for (auto it = aJson.begin(); it != aJson.end(); it++)
		{
			ImguiJson(it.key(), it.value());
		}
	}
	else if(aJson.is_boolean())
	{
		ImGui::Checkbox(aKey.c_str(), aJson.get_ptr<bool*>());
	}
	else if (aJson.is_number_float())
	{
		ImGui::InputDouble(aKey.c_str(), aJson.get_ptr<json::number_float_t*>());
	}
	else if (aJson.is_number_integer())
	{
		int value = static_cast<int>(aJson.get<json::number_integer_t>());
		ImGui::InputInt(aKey.c_str(), &value);
		aJson = value;
	}
	else if (aJson.is_number_unsigned())
	{
		int value = static_cast<int>(aJson.get<json::number_unsigned_t>());
		ImGui::InputInt(aKey.c_str(), &value);
		aJson = value;
	}
	else if (aJson.is_string())
	{
		ImGui::InputText(aKey.c_str(), aJson.get_ptr<json::string_t*>());
	}
	else if (aJson.is_null())
	{
		ImGui::Text(aKey.c_str());
	}
	else
	{
		WmLog("Unknown json type.");
	}
}

inline void ImguiObject(Object& aObject)
{
	SystemPtr<SerializationSystem> serializationSystem;

	auto json = serializationSystem->Serialize(aObject);

	ImGui::Begin("Object Inspector");

	for (auto& element : json)
	{
		ImguiJson(element["type"], element["data"]);
	}

	ImGui::End();

	// (Kevin): This is causing a bug where the ref count of components and functionalities are increased every frame!
	serializationSystem->Deserialize(json, aObject);
}

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

void GameWorld::Tick() noexcept
{
}

void GameWorld::Debug()
{
	ImguiObject(myPlayer);
}
