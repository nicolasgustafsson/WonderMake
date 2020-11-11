#include "pch.h"
#include "Navmesh.h"

void Navmesh::GenerateFromPolygon(const Geometry::Polygon& aPolygon)
{
	Geometry::Polygon copy = aPolygon;

	myPolygon = aPolygon;

	auto firstPoint = copy.FirstPoint();

	while (copy.AmountOfPoints() > 2)
		AddTriangle(aPolygon.TriangulateStep(copy, firstPoint));
	
	GenerateEdges();

	for (auto&& line : myLines)
	{
		WmDrawDebugLine(line.Line.First, line.Line.Second, SColor::Red, 1000.f);
	}

	for (auto&& line : myBounds)
	{
		WmDrawDebugLine(line->Line.First, line->Line.Second, SColor::CornflowerBlue, 1000.f);
	}
}

std::vector<Geometry::STriangle> Navmesh::GetLineOfSightFrom(const SVector2f aPosition)
{
	std::vector<Geometry::STriangle> linesOfSight;

	Geometry::PolygonLoopingPointOperator point = myPolygon.FirstPoint();
	std::optional<SVector2f> previousPoint;
	std::optional<SVector2f> firstPoint;
	bool longTriangle = false;
	std::optional<Geometry::PolygonLoopingPointOperator> previousPointOp;
	do
	{
		SLine line{ aPosition, *point };
		if (myPolygon.IntersectsWithAnySide(line, point))
		{
			++point;
			continue;
		}

		//if ((*point).DistanceTo({ 730.f, 25.f }) < 1.f)
		//	WmLog("");

		SLine firstSide{ *point.GetNextSide().GetStart(), *point.GetNextSide().GetEnd() };
		SLine secondSide{ *point.GetPreviousSide().GetEnd(), *point.GetPreviousSide().GetStart() };
		
		SDegreeF32 firstRotation = firstSide.GetRotation();
		SDegreeF32 secondRotation = secondSide.GetRotation();
		SDegreeF32 lineTestRotation = line.GetRotation();

		SDegreeF32 minRotation = std::min(firstRotation, secondRotation);
		SDegreeF32 maxRotation = std::max(firstRotation, secondRotation);

		if (minRotation - maxRotation < -180.f)
		{
			maxRotation -= 360.f;
			std::swap(minRotation, maxRotation);
		}

		if (minRotation < 0.f)
		{
			minRotation += 360.f;
			maxRotation += 360.f;
		}

		if (lineTestRotation < 0.f)
		{
			lineTestRotation += 360.f;
		}

		SDegreeF32 lineTestRotationInverse = lineTestRotation;
		SDegreeF32 lineTestRotationInverse2 = lineTestRotation;
		lineTestRotationInverse -= 180.f;
		lineTestRotationInverse2 += 180.f;
		const bool stopTracing = (lineTestRotation > minRotation && lineTestRotation < maxRotation)
			|| (lineTestRotationInverse > minRotation && lineTestRotationInverse < maxRotation)
			|| (lineTestRotationInverse2 > minRotation && lineTestRotationInverse2 < maxRotation);
		
		if (!previousPoint)
		{

			if (!stopTracing)
			{
				const SVector2f previousPointTemp = *previousPoint;
				previousPoint = line.Second;
				line.ExtendEnd(10000.f);

				if (auto intersection = myPolygon.IntersectsWithAnySide(line, point, true))
					line.Second = *intersection;
				//linesOfSight.push_back({ aPosition, line.Second, previousPointTemp });
				firstPoint = line.Second;
			}
			else
			{
				firstPoint = line.Second;
				previousPoint = line.Second;
			}
		}
		else if (!stopTracing)
		{
			auto pointCopy = point;
			pointCopy--;

			const bool shouldCreateShortTriangle = (previousPointOp == pointCopy);
			if (shouldCreateShortTriangle)
			{
				linesOfSight.push_back({ aPosition, line.Second, *previousPoint });
				line.ExtendEnd(10000.f);

				if (auto intersection = myPolygon.IntersectsWithAnySide(line, point, true))
					line.Second = *intersection;
				previousPoint = line.Second;
			}
			else
			{
				const SVector2f previousPointTemp = *previousPoint;
				previousPoint = line.Second;
				line.ExtendEnd(10000.f);

				if (auto intersection = myPolygon.IntersectsWithAnySide(line, point, true))
					line.Second = *intersection;
				linesOfSight.push_back({ aPosition, line.Second, previousPointTemp });
			}
		}
		else
		{
			linesOfSight.push_back({ aPosition, line.Second, *previousPoint });
			previousPoint = line.Second;
		}
		previousPointOp = point;

		++point;
	} while (point != myPolygon.FirstPoint());

	linesOfSight.push_back({ aPosition, *previousPoint, *firstPoint });

	return linesOfSight;
}

void Navmesh::AddTriangle(const Geometry::STriangle& aTriangle)
{
	SNavmeshTriangle& addedTriangle = *myTriangles.insert({aTriangle});

	AddPoint(aTriangle.First);
	AddPoint(aTriangle.Second);
	AddPoint(aTriangle.Third);
	AddLine({ aTriangle.First, aTriangle.Second }, &addedTriangle);
	AddLine({ aTriangle.First, aTriangle.Third }, &addedTriangle);
	AddLine({ aTriangle.Third, aTriangle.Second }, &addedTriangle);
}

void Navmesh::AddPoint(const SVector2f aPoint)
{
	if (std::find(myPoints.begin(), myPoints.end(), aPoint) != myPoints.end())
		return;

	myPoints.insert(aPoint);
}

void Navmesh::AddLine(const SLine aLine, SNavmeshTriangle* aTriangle)
{
	auto foundLine = std::find_if(myLines.begin(), myLines.end(), [aLine](const SNavmeshLine& aNavmeshLine) {return aNavmeshLine.Line == aLine; });
	if (foundLine != myLines.end())
	{
		foundLine->SecondTriangle = aTriangle;

		if (!aTriangle->HasLine(*foundLine))
		{
			if (!aTriangle->FirstSide)
				aTriangle->FirstSide = &(*foundLine);
			else if (!aTriangle->SecondSide)
				aTriangle->SecondSide = &(*foundLine);
			else if (!aTriangle->ThirdSide)
				aTriangle->ThirdSide = &(*foundLine);
		}
		else
		{
			WmLog("wee");
		}

		return;
	}

	auto insertedLine = myLines.insert({ aLine, aTriangle });

	if (!aTriangle->FirstSide)
		aTriangle->FirstSide = &(*insertedLine);
	else if (!aTriangle->SecondSide)
		aTriangle->SecondSide = &(*insertedLine);
	else if (!aTriangle->ThirdSide)
		aTriangle->ThirdSide = &(*insertedLine);
}

void Navmesh::GenerateEdges()
{
	for (auto&& line : myLines)
	{
		if (!line.SecondTriangle)
			myBounds.insert(&line);
	}
}

const plf::colony<SNavmeshTriangle>& Navmesh::GetTriangles() const
{
	return myTriangles;
}
