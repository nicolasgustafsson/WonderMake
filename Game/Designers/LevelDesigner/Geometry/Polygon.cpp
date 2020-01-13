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
}