#include "pch.h"

#include "CollisionSystem.h"
#include "CollisionFunctionality.h"
#include "Collision/Colliders.h"
#include "Utilities/Geometry/Intersections.h"
#include "Scheduling/ScheduleSystem.h"

REGISTER_UNIVERSE_SYSTEM(CollisionSystem);

CollisionSystem::CollisionSystem() noexcept
{
	Get<ScheduleSystem>().ScheduleRepeating<>([this](){ Tick(); });
	WmLog("Created collision system!!");
}

void CollisionSystem::Tick()
{
	plf::colony<std::function<void()>> reactionsToTrigger;

	for (Colliders::Shape* shape : myCollidersWithReactions)
	{
		Colliders::Shape& shapeRef = *shape;
		std::visit([&](const auto& aCollider)
			{
				//should iterate backwards ? alt. change to colony
				for (const Colliders::SReaction& reaction : aCollider.Reactions)
				{
					reactionsToTrigger.insert([this, shapeRef, reaction]()
					{
						OverlapAgainstFunctionalityInternal(shapeRef, reaction);
					});
				}
			}, shapeRef);
	}

	for (auto&& reaction : reactionsToTrigger)
	{
		reaction();
	}
}

bool CollisionSystem::DestroyCollider(Colliders::Shape& aCollider)
{
	std::visit([&](const auto& aShape)
		{
			if (aShape.Reactions.size() > 0)
			{
				myCollidersWithReactions.erase(std::find(myCollidersWithReactions.begin(), myCollidersWithReactions.end(), &aCollider));
			}
			myCollidersByType[aShape.Identifier].erase(myCollidersByType[aShape.Identifier].get_iterator_from_pointer(&aCollider));
		}, aCollider);

	return true;
}

bool CollisionSystem::TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept
{
	//don't have a collider overlap itself
	if (&aColliderB == &aColliderA)
		return false;

	return std::visit([&aColliderB](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereCollision(aCollider, aColliderB);
			}
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
			{
				return TestLineCollision(aCollider, aColliderB);
			}
			else
			{
				//static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aColliderA);
}

bool CollisionSystem::TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aSphere](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereVsSphereCollision(aSphere, aCollider);
			}
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
			{
				return TestSphereVsLineCollision(aSphere, aCollider);
			}
			else
			{
				//static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}

bool CollisionSystem::TestLineCollision(const Colliders::SCollisionLine& aLine, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aLine](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
			{
				return TestLineVsLineCollision(aCollider, aLine);
			}
			else if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereVsLineCollision(aCollider, aLine);
			}
			else
			{
				//static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}

bool CollisionSystem::TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept
{
    return WmIntersections2D::CircleVsCircle({aSphereA.Position, aSphereA.Radius}, {aSphereB.Position, aSphereB.Radius});
}

void CollisionSystem::OverlapAgainstFunctionalityInternal(const Colliders::Shape& aCollider, const Colliders::SReaction& aReaction)
{
	auto colonyIt = myCollidersByType.find(aReaction.Filter);

	if (colonyIt == myCollidersByType.end())
		return;

	plf::colony<Colliders::Shape>& colony = (*colonyIt).second;

	for (Colliders::Shape& shape : colony)
	{
		if (TestCollision(shape, aCollider))
		{
			std::visit([&](auto&& aShape)
				{
					Colliders::SCollisionInfo collisionInfo{ aCollider, shape };
					aReaction.Callback(*aShape.Functionality, collisionInfo);
				}, shape);
		}
	}
}

bool CollisionSystem::TestSphereVsLineCollision(const Colliders::SSphere& aSphereA, const Colliders::SCollisionLine& aLineB) noexcept
{
    return WmIntersections2D::CircleVsSurface({aSphereA.Position, aSphereA.Radius}, {aLineB.Position, aLineB.GetLineEnd(), aLineB.Width});
}

bool CollisionSystem::TestLineVsLineCollision(const Colliders::SCollisionLine& /*aLineA*/, const Colliders::SCollisionLine& /*aLineB*/) noexcept
{
	WmLog(TagError, "Line-line collision not implemented yet!");
	return false;
}
