#pragma once
#include "PolygonOperators/PolygonOperators.h"
#include "Utilities/Line.h"

class Navmesh;

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
		STriangle TriangulateStep(Polygon& aPolygon, PolygonLoopingPointOperator& aOperator) const;

		size_t AmountOfPoints() const { return myPoints.size(); }
		
		std::optional<std::pair<PolygonSideOperator, SVector2f>> IntersectsWithAnySide(const SLine aLine, std::optional<PolygonLoopingPointOperator> aIgnorePoint = {}, const bool aFindClosest = false);

		//[Nicos]: This is a general line - line intersection that we may want to move into the collision system in the future
		std::optional<SVector2f> Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, const SVector2f aThirdPoint, const SVector2f aFourthPoint, const f32 aTolerance = 0.f) const;
		std::optional<SVector2f> Intersects(const SVector2f aFirstPoint, const SVector2f aSecondPoint, PolygonSideOperator aOperator, const f32 aTolerance = 0.f) const;
	private:

		std::list<SVector2f> myPoints;

	};
}