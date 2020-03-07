#include "pch.h"
#include "BuffGiverFunctionality.h"
#include "Character/CharacterFunctionality.h"

BuffGiverFunctionality::BuffGiverFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{

}

void BuffGiverFunctionality::Initialize(BuffBlueprint& aBuffBlueprint, const f32 aRadius)
{
	SBuffGiverComponent& buffGiverComponent = Get<SBuffGiverComponent>();

	buffGiverComponent.BuffBlueprint = &aBuffBlueprint;

	buffGiverComponent.Collider = &Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), aRadius);

	Get<CollisionFunctionality>().AddReaction<CharacterFunctionality>(*buffGiverComponent.Collider, [&aBuffBlueprint](CharacterFunctionality& aCharacter, Colliders::SCollisionInfo)
		{
			CharacterBuffsFunctionality& buffFunctionality = aCharacter.Get<CharacterBuffsFunctionality>();

			if (!buffFunctionality.HasBuff(aBuffBlueprint))
			{
				buffFunctionality.ApplyBuff(aCharacter, aBuffBlueprint);
				WmLog("Gave the buff!");
			}
		});
}
