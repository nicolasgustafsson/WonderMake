#include "pch.h"
#include "FactionFunctionality.h"


FactionFunctionality::FactionFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void FactionFunctionality::SetFaction(const EFaction aFaction) noexcept
{
	Get<SFactionComponent>().Faction = aFaction;
}

bool FactionFunctionality::IsFriendlyWith(const EFaction aFaction) const noexcept
{
	return aFaction == Get<SFactionComponent>().Faction;
}

EFaction FactionFunctionality::GetFaction() const noexcept
{
	return Get<SFactionComponent>().Faction;
}