#include "pch.h"

#include "LevelDesigner.h"

#include "Collision/CollisionFunctionality.h"
#include "Collision/CollisionSystem.h"
#include "Enemy/EnemyControllerFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Randomizer/Randomizer.h"

Level LevelDesigner::DesignLevel()
{
	Level level;

	level.Enemies = DesignEnemies();

	return level;
}

plf::colony<Object> LevelDesigner::DesignEnemies() const
{
	plf::colony<Object> enemies;

	SystemPtr<Randomizer> randomizer;

	const auto enemyCount = randomizer->GetRandomNumber<size_t>(2, 5);

	for (size_t i = 0; i < enemyCount; ++i)
	{
		auto& enemy = *enemies.emplace();
		enemy.Add<EnemyControllerFunctionality>();
		auto& transform = enemy.Add<TransformFunctionality>();

		const SVector2f position{ randomizer->GetRandomNumber<f32>(-100, 100), randomizer->GetRandomNumber<f32>(-100, 100) };
		transform.SetPosition(position);
	}

	return enemies;
}
