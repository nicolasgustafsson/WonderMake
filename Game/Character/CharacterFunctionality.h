#pragma once
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Character/Stats/CharacterStatsFunctionality.h"
#include "Character/Buffs/CharacterBuffsFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "HitShapes/HitShapeSpawnerFunctionality.h"
#include "UtilityFunctionalities/FactionFunctionality.h"

class InputSystem;

class CharacterStatsFunctionality;

struct SHealthComponent : public  SComponent
{
	i32 MaxHealth = 200;
	i32 Health = MaxHealth;
};

class CharacterFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<InputSystem, PWrite>,
			PAdd<CollisionFunctionality, PWrite>,
			PAdd<SHealthComponent, PWrite>,
			PAdd<FactionFunctionality, PWrite>,
			PAdd<TransformFunctionality2D, PWrite>,
			PAdd<DefaultMovementFunctionality, PWrite>,
			PAdd<CharacterStatsFunctionality, PWrite>,
			PAdd<HitShapeSpawnerFunctionality, PWrite>,
			PAdd<LevelDenizenFunctionality, PWrite>,
			PAdd<CharacterBuffsFunctionality, PWrite>>>
{
public:
	CharacterFunctionality() noexcept;

	void Heal(const i32 aHealAmount);
	void Damage(const i32 aDamage);

	[[nodiscard]] bool IsDead() const noexcept;
	[[nodiscard]] bool IsAlive() const noexcept;

	void Inspect();
};
