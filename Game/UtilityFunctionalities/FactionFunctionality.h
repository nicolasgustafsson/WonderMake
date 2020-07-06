#pragma once

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

class FactionFunctionality : public Functionality<
	FactionFunctionality,
	Policy::Set<
	Policy::Add<SFactionComponent, Policy::EPermission::Write >> >
{
public:
	FactionFunctionality(Object& aOwner);
	void SetFaction(const EFaction aFaction) noexcept;
	[[nodiscard]] bool IsFriendlyWith(const EFaction aFaction) const noexcept;
	[[nodiscard]] EFaction GetFaction() const noexcept;
};


