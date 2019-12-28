#include "pch.h"
#include "LevelPortalFunctionality.h"

#include "GameWorld.h"

#include "Character/CharacterFunctionality.h"

LevelPortalFunctionality::LevelPortalFunctionality(Object& aOwner)
	: Super(aOwner)
{
	auto& collision = Get<CollisionFunctionality>();

	auto& collider = collision.AddSphereCollider(*this, SVector2f::Zero(), 32.f);

	collision.AddReaction<CharacterFunctionality>(collider, [&](auto& aCharacter)
		{
			if (myDoOnce)
			{
				return;
			}

			if (aCharacter.GetFaction() != EFaction::Player)
			{
				return;
			}

			WmDispatchTask([]() 
				{
					SystemPtr<GameWorld>()->RestartLevel();
				}, ERoutineId::Logic);
			myDoOnce = true;
		});
}

LevelPortalFunctionality::~LevelPortalFunctionality()
{

}
