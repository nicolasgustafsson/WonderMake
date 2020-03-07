#pragma once
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"
#include "Character/Stats/CharacterStatsFunctionality.h"
#include "Character/Buffs/CharacterBuffsFunctionality.h"

class CharacterStatsFunctionality;

struct SHealthComponent : public  SComponent
{
	i32 Health = 200;
};

struct SDiedImpulse
	: public SObjectImpulse<SDiedImpulse>
{

};

enum class EFaction
{
	Player,
	Enemy,
	Neutral
};

struct SFactionComponent : public SComponent
{
	EFaction Faction;
};

class CharacterFunctionality
	: public Functionality<CharacterFunctionality, OwnerFunctionality
	, CollisionFunctionality, SHealthComponent
	, SFactionComponent, TransformFunctionality
	, DefaultMovementFunctionality, CharacterStatsFunctionality
	, CharacterBuffsFunctionality>
{
public:
	CharacterFunctionality(Object& aOwner) noexcept;

	void Damage(const i32 aDamage);

	[[nodiscard]] bool IsDead() const noexcept;
	[[nodiscard]] bool IsAlive() const noexcept;
	//Nicos: might want to move faction code to its own functionality in the future
	void SetFaction(const EFaction aFaction) noexcept;
	[[nodiscard]] bool IsFriendlyWith(const EFaction aFaction) const noexcept;
	[[nodiscard]] EFaction GetFaction() const noexcept;
};

