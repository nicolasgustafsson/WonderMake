#pragma once
#include "PolygonPointOperator.h"

namespace Geometry
{
	class PolygonSideOperator
	{
	public:
		friend class Polygon;
		friend class PolygonFloatingOperator;
		friend class PolygonPointOperator;
		friend class PolygonPointOperatorBase;

		PolygonSideOperator& operator++()
		{
			++myBaseOperator;
			return *this;
		}

		PolygonSideOperator& operator--()
		{
			--myBaseOperator;
			return *this;
		}

		PolygonLoopingPointOperator GetStart() const;
		PolygonLoopingPointOperator GetEnd() const;

		f32 Length() const
		{
			const f32 length = (*GetStart() - *GetEnd()).Length();
			return length;
		}

		bool operator==(const PolygonSideOperator& aOther) const { return aOther.myBaseOperator == myBaseOperator; };
		bool operator!=(const PolygonSideOperator& aOther) const { return !(aOther == *this); };

		std::list<SVector2f>::iterator GetListIterator() const { myBaseOperator.GetListIterator(); }

		PolygonSideOperator Extrude(const f32 aDistance);

		SVector2f GetNormal() const;

		class PolygonFloatingOperator GetMiddle() const;

		PolygonSideOperator Move(const SVector2f aDistance);

		PolygonSideOperator GetNextSide() const;
		PolygonSideOperator GetPreviousSide() const;

	private:

		PolygonSideOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList);

		PolygonLoopingPointOperator myBaseOperator;
	};

};

