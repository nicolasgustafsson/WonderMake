#pragma once
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"
#include "Designers/LevelDesigner/Geometry/Polygon.h"
#include "Utilities/Geometry/Line.h"

struct SNavmeshLine;

struct SNavmeshTriangle
{
	Geometry::STriangle Triangle;
	SNavmeshLine* FirstSide = nullptr;
	SNavmeshLine* SecondSide = nullptr;
	SNavmeshLine* ThirdSide = nullptr;

	bool HasLine(const SNavmeshLine& aLine) const { return FirstSide == &aLine || SecondSide == &aLine || ThirdSide == &aLine; }
};

struct SNavmeshLine
{
	SLine Line;
	SNavmeshTriangle* FirstTriangle = nullptr;
	SNavmeshTriangle* SecondTriangle = nullptr;
};

class Navmesh
{
public:
	void GenerateFromPolygon(const Geometry::Polygon& aPolygon);
	
	std::vector<Geometry::STriangle> GetLineOfSightFrom(const SVector2f aPosition); //[Nicos]: todo make const

	const plf::colony<SNavmeshTriangle>& GetTriangles() const;
	const Geometry::Polygon& GetPolygon() const { return myPolygon; }

private:
	void AddTriangle(const Geometry::STriangle& aTriangle);
	void AddPoint(const SVector2f aPoint);
	void AddLine(const SLine aLine, SNavmeshTriangle* aTriangle);

	void GenerateEdges();

	plf::colony<SNavmeshTriangle> myTriangles;
	plf::colony<SVector2f> myPoints;
	plf::colony<SNavmeshLine> myLines;

	Geometry::Polygon myPolygon;

	plf::colony<SNavmeshLine*> myBounds;
};

