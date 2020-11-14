#include "pch.h"
#include "NavmeshFunctionality.h"
#include "Utility/Palette.h"

NavmeshFunctionality::NavmeshFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void NavmeshFunctionality::SetNavmesh(Navmesh aNavmesh)
{
	auto polygon = aNavmesh.GetPolygon();
	std::vector<SVector2f> boundsVec;
	auto&& point = polygon.FirstPoint();

	do 
	{
		boundsVec.push_back(*point);
		point++;
	} while (point != polygon.FirstPoint());

	Get<SNavmeshComponent>().Background.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(0.f, 0.f), -1003, Palette::MediumBackground, -0.8f);
	Get<SNavmeshComponent>().WallLines.emplace(boundsVec, 20.f, Palette::DarkBackground);
	Get<SNavmeshComponent>().WallLines->SetRenderLayer("Walls");
	Get<SNavmeshComponent>().Navmesh = std::move(aNavmesh);
}

void NavmeshFunctionality::Tick()
{
	if (!Get<SNavmeshComponent>().Background || !Get<SNavmeshComponent>().WallLines)
		return;

	Get<SNavmeshComponent>().Background->Render();
	Get<SNavmeshComponent>().WallLines->Render();
}
