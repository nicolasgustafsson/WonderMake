#pragma once
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Character/Stats/CharacterStatsFunctionality.h"
#include "Character/Buffs/CharacterBuffsFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "HitShapes/HitShapeSpawnerFunctionality.h"
#include "UtilityFunctionalities/FactionFunctionality.h"

class CharacterStatsFunctionality;

struct SHealthComponent : public  SComponent
{
	i32 MaxHealth = 200;
	i32 Health = MaxHealth;
};

struct SDiedImpulse
	: public SObjectImpulse<SDiedImpulse>
{

};

class CharacterFunctionality
	: public Functionality<
		CharacterFunctionality,
		Policy::Set<
			Policy::Add<OwnerFunctionality, Policy::EPermission::Write>,
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>,
			Policy::Add<SHealthComponent, Policy::EPermission::Write>,
			Policy::Add<FactionFunctionality, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<DefaultMovementFunctionality, Policy::EPermission::Write>,
			Policy::Add<CharacterStatsFunctionality, Policy::EPermission::Write>,
			Policy::Add<HitShapeSpawnerFunctionality, Policy::EPermission::Write>,
			Policy::Add<LevelDenizenFunctionality, Policy::EPermission::Write>,
			Policy::Add<CharacterBuffsFunctionality, Policy::EPermission::Write>>>
{
public:
	CharacterFunctionality(Object& aOwner) noexcept;

	void Heal(const i32 aHealAmount);
	void Damage(const i32 aDamage);

	[[nodiscard]] bool IsDead() const noexcept;
	[[nodiscard]] bool IsAlive() const noexcept;

	void Inspect();
};

REGISTER_FUNCTIONALITY(CharacterFunctionality);
