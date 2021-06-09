#include "pch.h"
#include "NavmeshFunctionality.h"
#include "Utility/Palette.h"

REGISTER_COMPONENT(SNavmeshComponent);
REGISTER_FUNCTIONALITY(NavmeshFunctionality);

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

	Get<SNavmeshComponent>().Background.emplace(aNavmesh, static_cast<u32>(aNavmesh.GetTriangles().size()), SVector2f(0.f, 0.f), -1003, Palette::MediumBackground, -0.8f);
	Get<SNavmeshComponent>().WallLines.emplace(boundsVec, 10.f, 10.f, Palette::DarkBackground);
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
