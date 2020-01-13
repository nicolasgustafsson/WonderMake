#pragma once
#include "PolygonPointOperator.h"
#include "PolygonSideOperator.h"

namespace Geometry
{
	class PolygonSideOperator;

	class PolygonFloatingOperator
	{
	public:
		friend class Polygon;
		friend class PolygonSideOperator;
		friend class PolygonPointOperator;

		PolygonFloatingOperator& operator+=(const f32 aDistance)
		{
			if (aDistance < 0.f)
			{
				(*this) -= -aDistance;
				return *this;
			}

			myCurrentDistance += aDistance;

			f32 length = myBaseOperator.GetNextSide().Length();

			while (length < myCurrentDistance)
			{
				myCurrentDistance -= length;

				++myBaseOperator;

				length = myBaseOperator.GetNextSide().Length();
			}

			return *this;
		}

		PolygonFloatingOperator& operator-=(const f32 aDistance)
		{
			if (aDistance < 0.f)
			{
				(*this) += -aDistance;
				return *this;
			}

			myCurrentDistance -= aDistance;

			while (myCurrentDistance < 0.f)
			{
				myCurrentDistance += myBaseOperator.GetPreviousSide().Length();

				--myBaseOperator;
			}

			return *this;
		}

		void GoToNextSide()
		{
			myCurrentDistance = 0.f;
			++myBaseOperator;
		}

		void GoToPreviousSide()
		{
			myCurrentDistance = 0.f;
			--myBaseOperator;
		}

		bool IsOnSameSide(const PolygonFloatingOperator& aOther) const
		{
			return aOther.myBaseOperator == myBaseOperator;
		}

		f32 CalculateDistance(const PolygonFloatingOperator& aEnd) const;

		PolygonSideOperator GetSide() const;

		PolygonLoopingPointOperator CreatePointAtLocation()
		{
			myBaseOperator.myBaseIterator = myBaseOperator.myList.insert(++myBaseOperator.myBaseIterator, GetLocation());

			myCurrentDistance = 0.f;

			return myBaseOperator;
		}

		SVector2f GetLocation() const
		{
			auto side = myBaseOperator.GetNextSide();
			const SVector2f start = *side.GetStart();
			const SVector2f end = *side.GetEnd();

			return start + (end - start).GetNormalized() * myCurrentDistance;
		}

		//Nicos: will invalidate this
		PolygonSideOperator SplitIntoSide(const f32 aSideLength);

		std::list<SVector2f>::iterator GetListIterator() const { myBaseOperator.GetListIterator(); }

	private:
		PolygonFloatingOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList);

		PolygonLoopingPointOperator myBaseOperator;
		f32 myCurrentDistance = 0.f;
	};
};

