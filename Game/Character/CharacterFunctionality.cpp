#include "pch.h"
#include "CharacterFunctionality.h"

CharacterFunctionality::CharacterFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{
	auto& collider = Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 15.f);

	Get<CollisionFunctionality>().AddReaction<CharacterFunctionality>(collider, [this](CharacterFunctionality& aOther, Colliders::SCollisionInfo)
		{
			const SVector2f selfPosition = Get<TransformFunctionality>().GetPosition();
			const SVector2f otherPosition = aOther.Get<TransformFunctionality>().GetPosition();

			SVector2f direction = (otherPosition - selfPosition).GetNormalized();
			const f32 distance = selfPosition.DistanceTo(otherPosition);
			aOther.Get<DefaultMovementFunctionality>().AddForce(direction * 100.f * (30.f - distance) * 0.5f);
		});

	Get<CharacterStatsFunctionality>().Get<SCharacterStatsComponent>().Character = this;
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
