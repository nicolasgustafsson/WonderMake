#include "pch.h"
#include "PolygonSideOperator.h"
#include "PolygonFloatingPointOperator.h"

namespace Geometry
{
	PolygonSideOperator::PolygonSideOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList)
		: myBaseOperator(aIterator, aOriginalList) {}


	PolygonLoopingPointOperator PolygonSideOperator::GetStart() const
	{
		PolygonLoopingPointOperator loopingIterator(myBaseOperator.myBaseIterator, myBaseOperator.myList);
		return loopingIterator;
	}

	PolygonLoopingPointOperator PolygonSideOperator::GetEnd() const
	{
		PolygonLoopingPointOperator loopingIterator(myBaseOperator.myBaseIterator, myBaseOperator.myList);
		++loopingIterator;
		return loopingIterator;
	}

	PolygonSideOperator PolygonSideOperator::Extrude(const f32 aDistance)
	{
		const SVector2f normal = GetNormal();

		auto start = GetStart();
		auto end = GetEnd();

		start.PrependPoint();
		end.AppendPoint();

		myBaseOperator = start;

		*start += normal * aDistance;
		*end += normal * aDistance;

		return *this;
	}

	SVector2f PolygonSideOperator::GetNormal() const
	{
		const SVector2f direction = (*GetEnd() - *GetStart()).GetNormalized();
		const SVector2f normal {-direction.Y, -direction.X };

		return normal;
	}

	PolygonFloatingOperator PolygonSideOperator::GetMiddle() const
	{
		PolygonFloatingOperator floatingPoint(myBaseOperator.myBaseIterator, myBaseOperator.myList);

		floatingPoint += Length() * 0.5f;

		return floatingPoint;
	}

	PolygonSideOperator PolygonSideOperator::Move(const SVector2f aMoveDelta)
	{
		GetStart().Move(aMoveDelta);
		GetEnd().Move(aMoveDelta);

		return *this;
	}

	PolygonSideOperator PolygonSideOperator::GetNextSide() const
	{
		PolygonSideOperator copy = *this;

		++copy;

		return copy;
	}

	PolygonSideOperator PolygonSideOperator::GetPreviousSide() const
	{
		PolygonSideOperator copy = *this;

		--copy;

		return copy;
	}
}
