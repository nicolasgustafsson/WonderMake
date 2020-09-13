#include "pch.h"
#include "FactionFunctionality.h"

REGISTER_COMPONENT(SFactionComponent);
REGISTER_FUNCTIONALITY(FactionFunctionality);

FactionFunctionality::FactionFunctionality(Object& aOwner, Dependencies&& aDependencies)
	: Super(aOwner, std::move(aDependencies))
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