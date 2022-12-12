#include "pch.h"

#include "CollisionSystem.h"
#include "CollisionFunctionality.h"
#include "Collision/Colliders.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Geometry.h"

WM_REGISTER_SYSTEM(CollisionSystem);

CollisionSystem::CollisionSystem() noexcept
{
	Get<ScheduleSystem>()
		.ScheduleRepeating(GetExecutor(), [this](){ Tick(); })
		.Detach();
}

void CollisionSystem::Tick()
{
	for (Colliders::Shape* shape : myCollidersWithReactions)
	{
		Colliders::Shape& shapeRef = *shape;
		std::visit([&](const auto& aCollider)
			{
				//should iterate backwards ? alt. change to colony
				for (const Colliders::SReaction& reaction : aCollider.Reactions)
				{
					OverlapAgainstFunctionalityInternal(shapeRef, reaction);
				}
			}, shapeRef);
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
				return TestSphereCollision(aCollider, aColliderB);
			else if constexpr (std::is_same_v<T, Colliders::SAxisAlignedBoundingBox>)
				return TestAABBCollision(aCollider, aColliderB);
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
				return TestLineCollision(aCollider, aColliderB);
			else
				[]<bool flag = false> { static_assert(flag, "Collider not implemented."); }();
		}, aColliderA);
}

bool CollisionSystem::TestSphereCollision(const Colliders::SSphere& aSphere, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aSphere](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
				return TestSphereVsSphereCollision(aSphere, aCollider);
			else if constexpr (std::is_same_v<T, Colliders::SAxisAlignedBoundingBox>)
				return TestSphereVsAABBCollision(aSphere, aCollider);
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
				return TestSphereVsLineCollision(aSphere, aCollider);
			else
				[]<bool flag = false> { static_assert(flag, "Collider not implemented."); }();
		}, aCollider);
}

bool CollisionSystem::TestAABBCollision(const Colliders::SAxisAlignedBoundingBox& aAABB, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aAABB](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
				return TestSphereVsAABBCollision(aCollider, aAABB);
			else if constexpr (std::is_same_v<T, Colliders::SAxisAlignedBoundingBox>)
				return false; // Not implememted.
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
				return false; // Not implememted.
			else
				[]<bool flag = false> { static_assert(flag, "Collider not implemented."); }();
		}, aCollider);
}

bool CollisionSystem::TestLineCollision(const Colliders::SCollisionLine& aLine, const Colliders::Shape& aCollider) noexcept
{
	return std::visit([aLine](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
				return TestLineVsLineCollision(aCollider, aLine);
			else if constexpr (std::is_same_v<T, Colliders::SAxisAlignedBoundingBox>)
				return false; // Not implememted.
			else if constexpr (std::is_same_v<T, Colliders::SSphere>)
				return TestSphereVsLineCollision(aCollider, aLine);
			else
				[]<bool flag = false> { static_assert(flag, "Collider not implemented."); }();
		}, aCollider);
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

bool CollisionSystem::TestSphereVsSphereCollision(const Colliders::SSphere& aSphereA, const Colliders::SSphere& aSphereB) noexcept
{
	const auto delta = aSphereA.Position - aSphereB.Position;

	return (delta.LengthSquared() <= std::powf(aSphereA.Radius + aSphereB.Radius, 2));
}

bool CollisionSystem::TestSphereVsAABBCollision(const Colliders::SSphere& aSphere, const Colliders::SAxisAlignedBoundingBox& aAABB) noexcept
{
	static constexpr auto getClosestPoint = [](SVector2f aPoint, const SRectangleF& aRectangle) -> SVector2f
	{
		static constexpr auto getX = [](SVector2f aPoint, const SRectangleF& aRectangle) -> f32
		{
			if (aPoint.X <= aRectangle.Left)
				return aRectangle.Left;
			if (aPoint.X >= aRectangle.Right)
				return aRectangle.Right;

			return aPoint.X;
		};
		static constexpr auto getY = [](SVector2f aPoint, const SRectangleF& aRectangle) -> f32
		{
			if (aPoint.Y <= aRectangle.Bottom)
				return aRectangle.Bottom;
			if (aPoint.Y >= aRectangle.Top)
				return aRectangle.Top;

			return aPoint.Y;
		};

		return { getX(aPoint, aRectangle), getY(aPoint, aRectangle) };
	};

	const SRectangleF rect
	{
		.Left	= aAABB.Position.X,
		.Top	= aAABB.Position.Y + aAABB.Dimensions.Y,
		.Right	= aAABB.Position.X + aAABB.Dimensions.X,
		.Bottom	= aAABB.Position.Y
	};
	
	const auto closestPoint = getClosestPoint(aSphere.Position, rect);

	return IsPointWithinSphere(aSphere, closestPoint);
}

bool CollisionSystem::TestSphereVsLineCollision(const Colliders::SSphere& aSphereA, const Colliders::SCollisionLine& aLineB) noexcept
{
	SVector2f closestPoint = GetClosestPointOnLine(aLineB, aSphereA.Position);

	const SVector2f lineDelta = aLineB.GetLineEnd() - aLineB.Position;

	const SVector2f lineNormal = lineDelta.GetPerpendicularClockWise().GetNormalized();
	
	//[Nicos]: create a newline that is perpendicular to the one line we have to simulate width
	Colliders::SCollisionLine newLine;
	newLine.Position = closestPoint - lineNormal * aLineB.Width;
	newLine.EndOffsetFromPosition = lineNormal * aLineB.Width * 2.f;

	closestPoint = GetClosestPointOnLine(newLine, aSphereA.Position);

	if (closestPoint.DistanceTo(aSphereA.Position) < aSphereA.Radius)
		return true;

	return false;
}

bool CollisionSystem::TestLineVsLineCollision(const Colliders::SCollisionLine& /*aLineA*/, const Colliders::SCollisionLine& /*aLineB*/) noexcept
{
	WmLogError(TagWonderMake << TagWmCollision << "Line-line collision not implemented yet.");
	return false;
}

bool CollisionSystem::IsPointWithinSphere(const Colliders::SSphere& aSphereA, const SVector2f aPoint) noexcept
{
	const auto delta = aSphereA.Position - aPoint;

	return (delta.LengthSquared() <= std::powf(aSphereA.Radius, 2));
}

SVector2f CollisionSystem::GetClosestPointOnLine(const Colliders::SCollisionLine& aLine, const SVector2f aPoint) noexcept
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

