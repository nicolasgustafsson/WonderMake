#pragma once
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"

struct SNavmeshTriangle
{
	Geometry::STriangle Triangle;
};

class Navmesh
{
public:
	void AddTriangle(const Geometry::STriangle& aTriangle);
	
	const plf::colony<SNavmeshTriangle>& GetTriangles() const;
private:
	plf::colony<SNavmeshTriangle> myTriangles;
};

