#include "pch.h"
#include "LevelPortalFunctionality.h"

#include "GameWorld.h"

#include "Character/CharacterFunctionality.h"

#include "Job/Job.h"

class RestartLevelJob
	: public Job<
		Policy::Set<
			PAdd<GameWorld, PWrite>>>
{
public:
	inline RestartLevelJob()
	{
		Get<GameWorld>().RestartLevel();
	}
};

REGISTER_FUNCTIONALITY(LevelPortalFunctionality);

LevelPortalFunctionality::LevelPortalFunctionality()
{
	auto& collision = Get<CollisionFunctionality>();

	auto& collider = collision.AddSphereCollider(*this, SVector2f::Zero(), 32.f);

	collision.AddReaction<CharacterFunctionality>(collider, [&](auto& aCharacter, Colliders::SCollisionInfo)
		{
			if (myDoOnce)
			{
				return;
			}

			if (aCharacter.Get<FactionFunctionality>().GetFaction() != EFaction::Player)
			{
				return;
			}

			RunJob<RestartLevelJob>();

			myDoOnce = true;
		});
}