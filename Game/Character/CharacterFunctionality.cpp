#include "pch.h"
#include "CharacterFunctionality.h"

CharacterFunctionality::CharacterFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{
	Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 10.f);
}

void CharacterFunctionality::Damage(const i32 aDamage)
{
	if (IsDead())
		return;
	
	Get<SHealthComponent>().Health -= aDamage;

	if (IsDead())
		WmSendObjectImpulse(Get<OwnerFunctionality>().GetOwner(), SDiedImpulse());
}

bool CharacterFunctionality::IsDead() const noexcept
{
	return Get<SHealthComponent>().Health <= 0;
}

bool CharacterFunctionality::IsAlive() const noexcept
{
	return !IsDead();
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
