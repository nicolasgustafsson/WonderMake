#pragma once
#include "PolygonOperators/PolygonOperators.h"

namespace Geometry
{
	class Polygon
	{
	public:
		PolygonLoopingPointOperator AppendPoint(const SVector2f aPoint);

		PolygonFloatingOperator GetIteratorOnRandomPoint();

		bool IsValidPolygon() const;

		PolygonLoopingPointOperator FirstPoint();
		PolygonLoopingPointOperator LastPoint();

		f32 CalculateCircumference();

	private:
		std::list<SVector2f> myPoints;
	};
}