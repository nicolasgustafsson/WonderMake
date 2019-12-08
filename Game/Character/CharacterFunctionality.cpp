#include "pch.h"
#include "CharacterFunctionality.h"

CharacterFunctionality::CharacterFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 10.f);
}

void CharacterFunctionality::SetFaction(const EFaction aFaction)
{
	Get<SFactionComponent>().Faction = aFaction;
}

bool CharacterFunctionality::IsFriendlyWith(const EFaction aFaction) const
{
	return aFaction == Get<SFactionComponent>().Faction;
}

EFaction CharacterFunctionality::GetFaction() const
{
	return Get<SFactionComponent>().Faction;
}
