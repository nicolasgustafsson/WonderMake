#include "pch.h"
#include "CharacterFunctionality.h"

CharacterFunctionality::CharacterFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{
	Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 10.f);
}

void CharacterFunctionality::SetFaction(const EFaction aFaction) noexcept
{
	Get<SFactionComponent>().Faction = aFaction;
}

bool CharacterFunctionality::IsFriendlyWith(const EFaction aFaction) const noexcept
{
	return aFaction == Get<SFactionComponent>().Faction;
}

EFaction CharacterFunctionality::GetFaction() const noexcept
{
	return Get<SFactionComponent>().Faction;
}
