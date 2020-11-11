#include "pch.h"
#include "Polygon.h"
#include "Randomizer/Randomizer.h"
#include <Utilities/Rotation.h>
#include "Physics/Navmesh/Navmesh.h"

float sign(SVector2f p1, SVector2f p2, SVector2f p3)
{
	return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
}

bool PointInTriangle(SVector2f pt, SVector2f v1, SVector2f v2, SVector2f v3)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
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

		SVector2f tangentFirst = triangle.First;
		SVector2f tangentSecond = triangle.Third;

		auto sideLooper = nextSide;
		++sideLooper;

		SVector2f firstRelative = triangle.First - triangle.Second;
		SVector2f thirdRelative = triangle.Third - triangle.Second;

		f32 dot = firstRelative.Dot(thirdRelative);
		f32 det = firstRelative.X * thirdRelative.Y - firstRelative.Y * thirdRelative.X;

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

	std::optional<SVector2f> Polygon::IntersectsWithAnySide(const SLine aLine, std::optional<PolygonLoopingPointOperator> aIgnorePoint, const bool aFindClosest)
	{
		auto firstSide = FirstPoint().GetNextSide();
		auto sideIt = firstSide;

		bool ignoreSide = sideIt.GetStart() == aIgnorePoint || sideIt.GetEnd() == aIgnorePoint;

		std::optional<SVector2f> closestIntersection;

		if (!ignoreSide)
		{
			if (auto intersection = Intersects(aLine.First, aLine.Second, sideIt))
			{
				if (!aFindClosest)
					return intersection;

				closestIntersection = intersection;
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
					if (!aFindClosest)
						return intersection;

					if (!closestIntersection || intersection->DistanceTo(aLine.First) < closestIntersection->DistanceTo(aLine.First))
						closestIntersection = intersection;
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
		return Intersects(aFirstPoint, aSecondPoint, *aOperator.GetStart(), *aOperator.GetEnd());
	}

}