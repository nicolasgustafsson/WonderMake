#include "pch.h"
#include "PolygonPointOperator.h"
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonSideOperator.h"

namespace Geometry
{
	PolygonPointOperatorBase::PolygonPointOperatorBase(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList)
		: myBaseIterator(aIterator), myList(aOriginalList) {}

	PolygonLoopingPointOperator::PolygonLoopingPointOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList)
		: PolygonPointOperatorBase(aIterator, aOriginalList) {}

	PolygonLoopingPointOperator::PolygonLoopingPointOperator(const PolygonPointOperatorBase& aOther)
		: PolygonPointOperatorBase(aOther) {}

	Geometry::PolygonSideOperator PolygonLoopingPointOperator::TurnIntoSide(PolygonLoopingPointOperator aEnd)
	{
		++(*this);

		while (*this != aEnd)
		{
			RemovePoint();
			//++(*this);
		}

		--(*this);

		return GetNextSide();
	}

	PolygonLoopingPointOperator& PolygonLoopingPointOperator::operator=(const PolygonLoopingPointOperator& aOther)
	{
		myBaseIterator = aOther.myBaseIterator;
		myList = aOther.myList;
		return *this;
	}

	PolygonSideOperator PolygonPointOperatorBase::GetNextSide() const
	{
		return PolygonSideOperator(myBaseIterator, myList);
	}

	PolygonSideOperator PolygonPointOperatorBase::GetPreviousSide() const
	{
		PolygonLoopingPointOperator loopingIterator(*this);
		--loopingIterator;

		return PolygonSideOperator(loopingIterator.myBaseIterator, loopingIterator.myList);
	}
}