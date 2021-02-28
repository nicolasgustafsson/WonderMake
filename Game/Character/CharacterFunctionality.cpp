#include "pch.h"
#include "CharacterFunctionality.h"
#include "Input/InputSystem.h"

REGISTER_COMPONENT(SHealthComponent);
REGISTER_FUNCTIONALITY(CharacterFunctionality);

CharacterFunctionality::CharacterFunctionality() noexcept
{
	auto& collider = Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 15.f);

	Get<CollisionFunctionality>().AddReaction<CharacterFunctionality>(collider, [this](CharacterFunctionality& aOther, Colliders::SCollisionInfo)
		{
			const SVector2f selfPosition = Get<TransformFunctionality2D>().GetPosition();
			const SVector2f otherPosition = aOther.Get<TransformFunctionality2D>().GetPosition();

			const SVector2f direction = (otherPosition - selfPosition).GetNormalized();
			const f32 distance = selfPosition.DistanceTo(otherPosition);
			aOther.Get<DefaultMovementFunctionality>().AddForce(direction * 100.f * (30.f - distance) * 0.5f);
		});

	Get<CharacterStatsFunctionality>().Get<SCharacterStatsComponent>().Character = this;
}

void CharacterFunctionality::Heal(const i32 aHealAmount)
{
	if (IsDead())
		return;

	Get<SHealthComponent>().Health += aHealAmount;
}

void CharacterFunctionality::Damage(const i32 aDamage)
{
	if (IsDead())
		return;
	
	Get<SHealthComponent>().Health -= aDamage;

	if (IsDead())
		{} // TODO: Kill character

	//Play on take damage effect

	//Get<EffectFunctionality>().ApplyEffect("GetHurtEffect");
	//SpawnEffect("HurtEffect");
}

bool CharacterFunctionality::IsDead() const noexcept
{
	return Get<SHealthComponent>().Health <= 0;
}

bool CharacterFunctionality::IsAlive() const noexcept
{
	return !IsDead();
}

void CharacterFunctionality::Inspect()
{
	auto& healthComponent = Get<SHealthComponent>();
	std::string healthString = ("Health ") + std::to_string(healthComponent.Health) + "/" + std::to_string(healthComponent.MaxHealth);

	ImGui::ProgressBar(static_cast<f32>(healthComponent.Health) / static_cast<f32>(healthComponent.MaxHealth), ImVec2(-1.f, 0.f), healthString.c_str());

	Get<CharacterBuffsFunctionality>().Inspect();
}

