#include "pch.h"
#include "LevelPortalFunctionality.h"

#include "Character/CharacterFunctionality.h"

LevelPortalFunctionality::LevelPortalFunctionality(Object& aOwner)
	: Super(aOwner)
{
	auto& collision = Get<CollisionFunctionality>();

	auto& collider = collision.AddSphereCollider(*this, SVector2f::Zero(), 32.f);

	collision.AddReaction<CharacterFunctionality>(collider, [](auto& aCharacter)
		{
			if (aCharacter.GetFaction() != EFaction::Player)
			{
				return;
			}

			WmLog("NOW YA DUMBING WITH PORTaLS");
		});
}
