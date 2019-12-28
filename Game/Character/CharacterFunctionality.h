#pragma once
#include "Functionalities/Functionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

struct SHealthComponent : public  SComponent
{
	f32 Health = 100;
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
	: public Functionality<CharacterFunctionality, CollisionFunctionality, SHealthComponent, SFactionComponent, TransformFunctionality>
{
public:
	CharacterFunctionality(Object& aOwner) noexcept;

	//Nicos: might want to move faction code to its own functionality in the future
	void SetFaction(const EFaction aFaction) noexcept;
	[[nodiscard]] bool IsFriendlyWith(const EFaction aFaction) const noexcept;
	[[nodiscard]] EFaction GetFaction() const noexcept;
};

