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
	for (Colliders::Shape* shape : myCollidersWithReactions)
	{
		Colliders::Shape& shapeRef = *shape;

		std::visit([&](auto&& aShape)
			{
				//should iterate backwards ? alt. change to colony
				for (Colliders::SReaction& reaction : aShape.Reactions)
				{
					OverlapAgainstFunctionalityInternal(aShape, reaction);
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
				myCollidersWithReactions.erase(std::find(myCollidersWithReactions.begin(), myCollidersWithReactions.end(), &aCollider));
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
			else if constexpr (std::is_same_v<T, Colliders::SLine>)
			{
				return TestLineCollision(aCollider, aColliderB);
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
			else if constexpr (std::is_same_v<T, Colliders::SLine>)
			{
				return TestSphereVsLineCollision(aSphere, aCollider);
			}
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}

bool CollisionSystem::TestLineCollision(const Colliders::SLine& aLine, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aLine](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SLine>)
			{
				return TestLineVsLineCollision(aCollider, aLine);
			}
			else if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				return TestSphereVsLineCollision(aCollider, aLine);
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
					aReaction.Callback(*aShape.Functionality);
				}, shape);
		}
	}
}

bool CollisionSystem::TestSphereVsLineCollision(const Colliders::SSphere& aSphereA, const Colliders::SLine& aLineB) noexcept
{
	const SVector2f closestPoint = GetClosestPointOnLine(aLineB, aSphereA.Position);

	if (closestPoint.DistanceTo(aSphereA.Position) < aSphereA.Radius)
		return true;

	return false;
}

bool CollisionSystem::TestLineVsLineCollision(const Colliders::SLine& aLineA, const Colliders::SLine& aLineB) noexcept
{
	WmLog(TagError, "Line-line collision not implemented yet!");
	return false;
}

bool CollisionSystem::IsPointWithinSphere(const Colliders::SSphere& aSphereA, const SVector2f aPoint) noexcept
{
	const auto delta = aSphereA.Position - aPoint;

	return (delta.LengthSquared() <= std::powf(aSphereA.Radius, 2));
}

SVector2f CollisionSystem::GetClosestPointOnLine(const Colliders::SLine& aLine, const SVector2f aPoint) noexcept
{
	//alghorithm based on one found here: https://stackoverflow.com/questions/3120357/get-closest-point-to-a-line
	const SVector2f lineStart = aLine.Position;
	const SVector2f lineEnd = aLine.GetLineEnd();

	if (lineStart == lineEnd)
		return lineStart;

	const SVector2f lineDelta = lineEnd - lineStart;
	const SVector2f sphereDeltaFromLineStart = aPoint - lineStart;

	const f32 lineDeltaMagnitude = lineDelta.LengthSquared();
	const f32 lineDeltaProduct = lineDelta.Dot(sphereDeltaFromLineStart);

	const f32 normalizedDistance = lineDeltaProduct / lineDeltaMagnitude; //distance on line to the closest point on it, from start to end

	if (normalizedDistance < 0.f)
		return lineStart; //clamp on line ends

	if (normalizedDistance > 1.f)
		return lineEnd;

	return lineStart + lineDelta * normalizedDistance;
}

