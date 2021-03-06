#pragma once

enum class EFaction
{
	Player,
	Enemy,
	Neutral
};

struct SFactionComponent : public SComponent
{
	EFaction Faction = EFaction::Neutral;
};

class FactionFunctionality : public Functionality<
	Policy::Set<
		PAdd<SFactionComponent, PWrite>>>
{
public:
	void SetFaction(const EFaction aFaction) noexcept;
	[[nodiscard]] bool IsFriendlyWith(const EFaction aFaction) const noexcept;
	[[nodiscard]] EFaction GetFaction() const noexcept;
};
