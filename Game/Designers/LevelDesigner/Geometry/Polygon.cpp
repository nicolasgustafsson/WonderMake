#include "pch.h"
#include "Polygon.h"
#include "Randomizer/Randomizer.h"
#include <Utilities/Rotation.h>

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

	void Polygon::StartTriangulate()
	{
		myTriangulationThing = new Polygon(*this);

		myTriangulationThingOp.emplace(myTriangulationThing->FirstPoint());
	}

	void Polygon::TriangulateStep()
	{
		if (!myTriangulationThing)
			return;

		PolygonLoopingPointOperator& pointOperator = *myTriangulationThingOp;
		STriangle triangle = pointOperator.GetTriangle();


		if (myTriangulationThing->myPoints.size() == 3)
		{
			WmDrawDebugLine(triangle.First, triangle.Second, SColor::Green, 100.f);
			WmDrawDebugLine(triangle.Second, triangle.Third, SColor::Green, 100.f);
			WmDrawDebugLine(triangle.First, triangle.Third, SColor::Green, 100.f);

			delete myTriangulationThing;

			myTriangulationThing = nullptr;

			return;
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
			TriangulateStep();
			return;
		}

		while (sideLooper != previousSide)
		{
			if (Intersects(tangentFirst, tangentSecond, *sideLooper.GetStart(), *sideLooper.GetEnd()))
			{
				++pointOperator;
				TriangulateStep();
				return;
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
				TriangulateStep();
				return;
			}

			++loopingPoint;
		}

		myLatestTriangle = triangle;

		WmDrawDebugLine(triangle.First, triangle.Second, SColor::Green, 100.f);
		WmDrawDebugLine(triangle.Second, triangle.Third, SColor::Green, 100.f);
		WmDrawDebugLine(triangle.First, triangle.Third, SColor::Green, 100.f);

		pointOperator.RemovePoint();
	}

	void Polygon::Draw()
	{

		if (!myTriangulationThing)
			return;

		auto side = myTriangulationThingOp->GetNextSide();

		auto loopingSide = side;

		SColor color = SColor{ 1.f, 0.f, 1.f, 1.f };
		WmDrawDebugLine(*loopingSide.GetStart(), *loopingSide.GetEnd(), color, 0.f);

		++loopingSide;
		while (loopingSide != side)
		{
			WmDrawDebugLine(*loopingSide.GetStart(), *loopingSide.GetEnd(), color, 0.f);
			++loopingSide;
		}

		WmDrawDebugLine(myLatestTriangle.First, myLatestTriangle.Second, SColor::Red, 0.f);
		WmDrawDebugLine(myLatestTriangle.Second, myLatestTriangle.Third, SColor::Red, 0.f);
		WmDrawDebugLine(myLatestTriangle.First, myLatestTriangle.Third, SColor::Red, 0.f);
	}

	bool Polygon::Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, const SVector2f aThirdPoint, const SVector2f aFourthPoint) const
	{
		//[Nicos]: shamelessly stolen from http://flassari.is/2008/11/line-line-intersection-in-cplusplus/
		
		auto p1 = aFirstPoint, p2 = aSecondPoint, p3 = aThirdPoint, p4 = aFourthPoint;
		// Store the values for fast access and easy
		// equations-to-code conversion
		float x1 = p1.X, x2 = p2.X, x3 = p3.X, x4 = p4.X;
		float y1 = p1.Y, y2 = p2.Y, y3 = p3.Y, y4 = p4.Y;

		float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If d is zero, there is no intersection
		if (d == 0) 
			return false;

		// Get the x and y
		float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
		float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
		float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

		// Check if the x and y coordinates are within both lines
		if (x < std::min(x1, x2) || x > std::max(x1, x2) ||
			x < std::min(x3, x4) || x > std::max(x3, x4)) return false;
		if (y < std::min(y1, y2) || y > std::max(y1, y2) ||
			y < std::min(y3, y4) || y > std::max(y3, y4)) return false;

		//x and y are the points of the intersection if we ever need them
		return true;
	}
}