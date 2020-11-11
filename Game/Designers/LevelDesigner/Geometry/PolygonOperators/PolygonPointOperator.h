#pragma once

namespace Geometry
{
	struct STriangle 
	{
		SVector2f First;
		SVector2f Second;
		SVector2f Third;
	};

	class PolygonPointOperatorBase
	{
	public:
		SVector2f& operator*() const { return *myBaseIterator; }

		SVector2f& operator->() const { return *myBaseIterator; }

		bool operator==(const PolygonPointOperatorBase& aOther) const { return aOther.myBaseIterator == myBaseIterator; };
		bool operator!=(const PolygonPointOperatorBase& aOther) const { return !(aOther == *this); };

		class PolygonSideOperator GetNextSide() const;

		class PolygonSideOperator GetPreviousSide() const;

		void Move(const SVector2f aMoveDelta)
		{
			*(*this) += aMoveDelta;
		}

	protected:
		std::list<SVector2f>::iterator GetListIterator() const { myBaseIterator; }
		PolygonPointOperatorBase(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList);
		std::list<SVector2f>::iterator myBaseIterator;
		std::list<SVector2f>& myList;
	};

	class PolygonLoopingPointOperator : public PolygonPointOperatorBase
	{
	public:
		friend class Polygon;
		friend class PolygonFloatingOperator;
		friend class PolygonSideOperator;

		PolygonLoopingPointOperator(const PolygonPointOperatorBase&);
		PolygonLoopingPointOperator& operator=(const PolygonLoopingPointOperator&);

		PolygonLoopingPointOperator& operator++()
		{
			if (myBaseIterator == --myList.end())
				myBaseIterator = myList.begin();
			else
				++myBaseIterator;

			return *this;
		}

		PolygonLoopingPointOperator& operator--()
		{
			if (myBaseIterator == myList.begin())
				myBaseIterator = myList.end();

			--myBaseIterator;

			return *this;
		}

		PolygonLoopingPointOperator operator++(int)
		{
			PolygonLoopingPointOperator temp = *this;

			++(*this);

			return temp;
		}

		PolygonLoopingPointOperator operator--(int)
		{
			PolygonLoopingPointOperator temp = *this;

			--(*this);

			return temp;
		}

		//returns prepended point
		PolygonLoopingPointOperator PrependPoint(const SVector2f aRelativeLocation = {0.f, 0.f})
		{ 
			const SVector2f location = *(*this) + aRelativeLocation;
			PolygonLoopingPointOperator newPoint(myList.insert(myBaseIterator, location), myList);

			return newPoint;
		}

		//returns appended point
		PolygonLoopingPointOperator AppendPoint(const SVector2f aRelativeLocation = { 0.f, 0.f })
		{
			const SVector2f location = *(*this) + aRelativeLocation;

			++myBaseIterator;
			PolygonLoopingPointOperator newPoint(myList.insert(myBaseIterator, location), myList);
			--myBaseIterator;
			--myBaseIterator;

			return newPoint;
		}

		PolygonLoopingPointOperator RemovePoint()
		{
			myBaseIterator = myList.erase(myBaseIterator);

			return *this;
		}

		PolygonSideOperator TurnIntoSide(PolygonLoopingPointOperator aEnd);

		STriangle GetTriangle();

	private:
		PolygonLoopingPointOperator(std::list<SVector2f>::iterator aIterator, std::list<SVector2f>& aOriginalList);
	};
}
