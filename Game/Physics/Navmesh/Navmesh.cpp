#include "pch.h"
#include "Navmesh.h"

void Navmesh::AddTriangle(const Geometry::STriangle& aTriangle)
{
	myTriangles.insert({aTriangle});
}

const plf::colony<SNavmeshTriangle>& Navmesh::GetTriangles() const
{
	return myTriangles;
}
