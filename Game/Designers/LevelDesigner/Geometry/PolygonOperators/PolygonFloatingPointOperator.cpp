#include "pch.h"
#include "PolygonFloatingPointOperator.h"
#include "PolygonSideOperator.h"

namespace Geometry
{
	PolygonSideOperator PolygonFloatingOperator::SplitIntoSide(const f32 aSideLength)
	{
		PolygonFloatingOperator start = *this;

		start -= aSideLength * 0.5f;

		PolygonLoopingPointOperator startPoint = start.CreatePointAtLocation();
		PolygonFloatingOperator end = PolygonFloatingOperator(start);

		end += aSideLength;

		PolygonLoopingPointOperator endPoint = end.CreatePointAtLocation();

		return startPoint.TurnIntoSide(endPoint);
	}

	PolygonFloatingOperator::PolygonFloatingOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList)
		:myBaseOperator(aIterator, aOriginalList) {}

	f32 Geometry::PolygonFloatingOperator::CalculateDistance(const PolygonFloatingOperator& aEnd) const
	{
		PolygonFloatingOperator start = *this;

		f32 distance = myCurrentDistance;
		start.myCurrentDistance = 0.f;

		do
		{
			distance += start.myCurrentDistance;
			start.GoToNextSide();
		} while (!start.IsOnSameSide(aEnd));

		distance += aEnd.myCurrentDistance;

		return distance;
	}

	PolygonSideOperator Geometry::PolygonFloatingOperator::GetSide() const
	{
		return PolygonSideOperator(myBaseOperator.myBaseIterator, myBaseOperator.myList);
	}
}
