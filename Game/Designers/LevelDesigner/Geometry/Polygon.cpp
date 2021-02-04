#include "pch.h"
#include "Polygon.h"
#include "Randomizer/Randomizer.h"
#include <Utilities/Rotation.h>
#include "Physics/Navmesh/Navmesh.h"

//[Nicos]: TODO move to separate library; Point In triangle should be part of a general function set
float Sign(const SVector2f aFirstPoint, const SVector2f aSecondPoint, const SVector2f aThirdPoint)
{
	return (aFirstPoint.X - aThirdPoint.X) * (aSecondPoint.Y - aThirdPoint.Y) - (aSecondPoint.X - aThirdPoint.X) * (aFirstPoint.Y - aThirdPoint.Y);
}

//[Nicos]: TODO move to separate library; Point In triangle should be part of a general function set
bool PointInTriangle(const SVector2f aPoint, const SVector2f aFirstTrianglePoint, const SVector2f aSecondTrianglePoint, const SVector2f aThirdTrianglePoint)
{
	const f32 demoninator1 = Sign(aPoint, aFirstTrianglePoint, aSecondTrianglePoint);
	const f32 demoninator2 = Sign(aPoint, aSecondTrianglePoint, aThirdTrianglePoint);
	const f32 demoninator3 = Sign(aPoint, aThirdTrianglePoint, aFirstTrianglePoint);

	const bool hasNegative = (demoninator1 < 0) || (demoninator2 < 0) || (demoninator3 < 0);
	const bool hasPositive = (demoninator1 > 0) || (demoninator2 > 0) || (demoninator3 > 0);

	return !(hasNegative && hasPositive);
}

namespace Geometry
{
	PolygonLoopingPointOperator Polygon::AppendPoint(const SVector2f aPoint)
	{
		myPoints.push_back(aPoint);
		return PolygonLoopingPointOperator(--myPoints.end(), myPoints);
	}

	PolygonFloatingOperator Polygon::GetIteratorOnRandomPoint()
	{
		SystemPtr<Randomizer> randomizer;

		PolygonFloatingOperator floatingIterator(myPoints.begin(), myPoints);

		const f32 distance = CalculateCircumference();

		floatingIterator += randomizer->GetRandomNumber(0.f, distance);

		return floatingIterator;
	}

	bool Polygon::IsValidPolygon() const
	{
		//check self intersections? 
		return myPoints.size() > 2;
	}

	PolygonLoopingPointOperator Polygon::FirstPoint()
	{
		return PolygonLoopingPointOperator(myPoints.begin(), myPoints);
	}

	PolygonLoopingPointOperator Polygon::LastPoint()
	{
		return PolygonLoopingPointOperator(--myPoints.end(), myPoints);
	}

	f32 Polygon::CalculateCircumference()
	{
		f32 circumference = 0.f;

		auto firstPoint = FirstPoint();

		auto side = firstPoint.GetNextSide();

		do
		{
			circumference += side.Length();
			++side;
		} while (side.GetStart() != firstPoint);

		return circumference;
	}

	STriangle Polygon::TriangulateStep(Polygon& aPolygon, PolygonLoopingPointOperator& aOperator) const
	{
		PolygonLoopingPointOperator& pointOperator = aOperator;
		STriangle triangle = pointOperator.GetTriangle();

		if (aPolygon.myPoints.size() == 3)
		{
			//WmDrawDebugLine(triangle.First, triangle.Second, SColor::Green, 100.f);
			//WmDrawDebugLine(triangle.Second, triangle.Third, SColor::Green, 100.f);
			//WmDrawDebugLine(triangle.First, triangle.Third, SColor::Green, 100.f);

			pointOperator.RemovePoint();

			return triangle;
		}

		auto nextSide = pointOperator.GetNextSide();
		auto previousSide = pointOperator.GetPreviousSide();

		++nextSide;
		--previousSide;

		const SVector2f tangentFirst = triangle.First;
		const SVector2f tangentSecond = triangle.Third;

		auto sideLooper = nextSide;
		++sideLooper;

		const SVector2f firstRelative = triangle.First - triangle.Second;
		const SVector2f thirdRelative = triangle.Third - triangle.Second;

		const f32 dot = firstRelative.Dot(thirdRelative);
		const f32 det = firstRelative.X * thirdRelative.Y - firstRelative.Y * thirdRelative.X;

		SRadianF32 angle(std::atan2f(det, dot));

		if (angle < 0.f)
		{
			++pointOperator;
			return TriangulateStep(aPolygon, pointOperator);
		}

		while (sideLooper != previousSide)
		{
			if (Intersects(tangentFirst, tangentSecond, *sideLooper.GetStart(), *sideLooper.GetEnd()))
			{
				++pointOperator;
				return TriangulateStep(aPolygon, pointOperator);
			}

			++sideLooper;
		}


		auto startPoint = previousSide.GetStart();
		auto endPoint = nextSide.GetEnd();

		auto loopingPoint = endPoint;

		while (loopingPoint != startPoint)
		{
			if (PointInTriangle(*loopingPoint, triangle.First, triangle.Second, triangle.Third))
			{
				++pointOperator;
				return TriangulateStep(aPolygon, pointOperator);
			}

			++loopingPoint;
		}

		//WmDrawDebugLine(triangle.First, triangle.Second, SColor::Green, 100.f);
		//WmDrawDebugLine(triangle.Second, triangle.Third, SColor::Green, 100.f);
		//WmDrawDebugLine(triangle.First, triangle.Third, SColor::Green, 100.f);

		pointOperator.RemovePoint();

		return triangle;
	}

	std::optional<std::pair<PolygonSideOperator, SVector2f>> Polygon::IntersectsWithAnySide(const SLine aLine, std::optional<PolygonLoopingPointOperator> aIgnorePoint, const bool aFindClosest)
	{
		auto firstSide = FirstPoint().GetNextSide();
		auto sideIt = firstSide;

		bool ignoreSide = sideIt.GetStart() == aIgnorePoint || sideIt.GetEnd() == aIgnorePoint;

		std::optional<std::pair<PolygonSideOperator, SVector2f>> closestIntersection;

		if (!ignoreSide)
		{
			if (auto intersection = Intersects(aLine.First, aLine.Second, sideIt))
			{
				std::pair<PolygonSideOperator, SVector2f> intersectionPair = { sideIt, *intersection };

				if (!aFindClosest)
					return intersectionPair;

				closestIntersection = intersectionPair;
			}
		}

		sideIt++;

		while (sideIt != firstSide)
		{
			ignoreSide = sideIt.GetStart() == aIgnorePoint || sideIt.GetEnd() == aIgnorePoint;

			if (!ignoreSide)
			{
				if (auto intersection = Intersects(aLine.First, aLine.Second, sideIt))
				{
					std::pair<PolygonSideOperator, SVector2f> intersectionPair = { sideIt, *intersection };
					if (!aFindClosest)
						return intersectionPair;

					if (!closestIntersection || intersectionPair.second.DistanceTo(aLine.First) < closestIntersection->second.DistanceTo(aLine.First))
						closestIntersection = intersectionPair;
				}
			}

			sideIt++;
		}

		return closestIntersection;
	}

	std::optional<SVector2f> Polygon::Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, const SVector2f aThirdPoint, const SVector2f aFourthPoint, const f32 aTolerance) const
	{
		//[Nicos]: shamelessly stolen from http://www.jeffreythompson.org/collision-detection/line-line.php
		
		const auto p1 = aFirstPoint, p2 = aSecondPoint, p3 = aThirdPoint, p4 = aFourthPoint;
		// Store the values for fast access and easy
		// equations-to-code conversion
		const f32 x1 = p1.X, x2 = p2.X, x3 = p3.X, x4 = p4.X;
		const f32 y1 = p1.Y, y2 = p2.Y, y3 = p3.Y, y4 = p4.Y;

		// calculate the distance to intersection point
		const f32 uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
		const f32 uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

		// if uA and uB are between 0-1, lines are colliding
		if (uA >= aTolerance && uA <= 1.f - aTolerance && uB >= aTolerance && uB <= 1.f - aTolerance) {

			const f32 intersectionX = x1 + (uA * (x2 - x1));
			const f32 intersectionY = y1 + (uA * (y2 - y1));

			return { {intersectionX, intersectionY} };
		}
		return std::nullopt;
	}

	std::optional<SVector2f> Polygon::Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, PolygonSideOperator aOperator, const f32 aTolerance /*= 0.f*/) const
	{
		return Intersects(aFirstPoint, aSecondPoint, *aOperator.GetStart(), *aOperator.GetEnd(), aTolerance);
	}

}