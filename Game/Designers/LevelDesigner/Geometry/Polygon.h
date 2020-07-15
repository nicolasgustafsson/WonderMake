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

		void StartTriangulate();

		void TriangulateStep();

		void Draw();

	private:
		//[Nicos]: This is a general line - line intersection that we may want to move into the collision system in the future
		bool Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, const SVector2f aThirdPoint, const SVector2f aFourthPoint) const;

		std::list<SVector2f> myPoints;

		Polygon* myTriangulationThing;

		std::optional<PolygonLoopingPointOperator> myTriangulationThingOp;

		STriangle myLatestTriangle;
	};
}