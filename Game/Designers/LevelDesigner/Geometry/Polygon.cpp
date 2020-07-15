#include "pch.h"
#include "Polygon.h"
#include "Randomizer/Randomizer.h"

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

	void Polygon::Triangulate()
	{
		Polygon copy = *this;

		PolygonLoopingPointOperator pointOperator = copy.FirstPoint();

		while (copy.myPoints.size() >= 3)
		{
			STriangle triangle = pointOperator.GetTriangle();

			SVector2f tangentFirst = triangle.First;
			SVector2f tangentThird = triangle.Third;

			WmDrawDebugLine(triangle.First, triangle.Second, SColor::Red, 100.f);
			WmDrawDebugLine(triangle.Second, triangle.Third, SColor::Red, 100.f);
			WmDrawDebugLine(triangle.First, triangle.Third, SColor::Red, 100.f);

			pointOperator.RemovePoint();
		}							
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