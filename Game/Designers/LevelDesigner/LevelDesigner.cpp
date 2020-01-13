#include "pch.h"

#include "LevelDesigner.h"

#include "Collision/CollisionFunctionality.h"
#include "Collision/CollisionSystem.h"
#include "Enemy/EnemyControllerFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Levels/LevelPortalFunctionality.h"
#include "Movement/SpinnerFunctionality.h"
#include "Randomizer/Randomizer.h"
#include "Physics/StaticGeometryFunctionality.h"

Level LevelDesigner::DesignLevel() const
{
	Level level;
	auto geometry = DesignGeometry();
	level.Objects.splice(geometry);

	auto enemies = DesignEnemies();
	level.Objects.splice(enemies);

	level.Objects.insert(DesignPortal());

	return level;
}

plf::colony<Object> LevelDesigner::DesignEnemies() const
{
	SystemPtr<Randomizer> randomizer;
	plf::colony<Object> enemies;

	const auto enemyCount = randomizer->GetRandomNumber<size_t>(2, 5);

	for (size_t i = 0; i < enemyCount; ++i)
	{
		auto& enemy = *enemies.emplace();
		enemy.Add<EnemyControllerFunctionality>();
		auto& transform = enemy.Add<TransformFunctionality>();

		const SVector2f position{ randomizer->GetRandomNumber<f32>(-300, 300), randomizer->GetRandomNumber<f32>(-300, 300) };
		transform.SetPosition(position);
	}

	return enemies;
}

Object LevelDesigner::DesignPortal() const
{
	SystemPtr<Randomizer> randomizer;
	Object portal;

	portal.Add<LevelPortalFunctionality>();
	portal.Add<SpinnerFunctionality>();
	auto& transform = portal.Add<TransformFunctionality>();
	auto& sprite = portal.Add<SpriteRenderingFunctionality>();

	SVector2f normal = { 1.f, 0.f };
	normal.Rotate(randomizer->GetRandomNumber<f32>(0, Constants::Tau));

	const SVector2f position = normal * randomizer->GetRandomNumber<f32>(150.f, 250.f);

	transform.SetPosition(position);
	sprite.SetTexture(std::filesystem::current_path() / "Textures/portal.png");

	return portal;
}

plf::colony<Object> LevelDesigner::DesignGeometry() const
{
	plf::colony<Object> geometry;

	auto room = DesignRoom();

	for (auto wall : room.Walls)
	{
		Object object;

		auto& geometryFunctionality = object.Add<StaticGeometryFunctionality>();

		geometryFunctionality.SetLine(wall.Start, wall.End);

		geometry.insert(std::move(object));
	}

	return geometry;
}

SRoom LevelDesigner::DesignRoom() const
{
	SystemPtr<Randomizer> randomizer;
	SRoom room;

	const f32 width = 400.f;
	const f32 height = 300.f;

	room.Walls.insert({ {-width, -height}, {-width, height} });
	room.Walls.insert({ {-width, -height}, {width, -height} });
	room.Walls.insert({ {width, height}, {-width, height} });
	room.Walls.insert({ {width, height}, {width, -height} });

	for (i32 i = 0; i < 10; i++)
	{
		const SVector2f startPosition{ randomizer->GetRandomNumber<f32>(-300, 300), randomizer->GetRandomNumber<f32>(-300, 300) };
		const SVector2f endPosition = startPosition + SVector2f{ randomizer->GetRandomNumber<f32>(-100, 100), randomizer->GetRandomNumber<f32>(-100, 100) };
		room.Walls.insert({ startPosition, endPosition });
	}

	return room;
}
