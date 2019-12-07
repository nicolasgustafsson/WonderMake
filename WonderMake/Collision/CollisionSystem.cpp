#include "pch.h"

#include "CollisionSystem.h"
#include "CollisionFunctionality.h"
#include "Collision/Colliders.h"

CollisionSystem::CollisionSystem() noexcept
{
	EnableTick();
}

void CollisionSystem::Tick() noexcept
{
	for (Colliders::Shape* shape : myCollidersWithCallbacks)
	{
		Colliders::Shape& shapeRef = *shape;

		std::visit([&](auto&& aShape)
			{
				//should iterate backwards ?
				for (Colliders::SReaction& reaction : aShape.Reactions)
				{
					OverlapAgainstFunctionality(aShape, reaction);
				}
			}, shapeRef);
	}
}

bool CollisionSystem::DestroyCollider(Colliders::Shape& aCollider)
{
	std::visit([&](auto&& aShape)
		{
			if (aShape.Reactions.size() > 0)
			{
				myCollidersWithCallbacks.erase(std::find(myCollidersWithCallbacks.begin(), myCollidersWithCallbacks.end(), &aCollider));
			}
			myCollidersByType[aShape.Identifier].erase(myCollidersByType[aShape.Identifier].get_iterator_from_pointer(&aCollider));
		}, aCollider);

	return true;
}

bool CollisionSystem::TestCollision(const Colliders::Shape& aColliderA, const Colliders::Shape& aColliderB) noexcept
{
	return std::visit([aColliderB](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereCollision(aCollider, aColliderB);
			}
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
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
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}

bool CollisionSystem::TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept
{
	const auto delta = aSphereA.Position - aSphereB.Position;
	
	return (delta.LengthSquared() <= std::powf(aSphereA.Radius + aSphereB.Radius, 2));
}

void CollisionSystem::OverlapAgainstFunctionality(const Colliders::Shape& aCollider, const Colliders::SReaction& aReaction)
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
					aReaction.Callback(*aShape.Functionality);
				}, shape);
		}
	}
}

