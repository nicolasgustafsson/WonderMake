#include "pch.h"
#include "NavmeshFunctionality.h"
#include "Utility/Palette.h"

NavmeshFunctionality::NavmeshFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void NavmeshFunctionality::SetNavmesh(Navmesh aNavmesh)
{
	Get<SNavmeshComponent>().Background.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(0.f, 0.f), -1003, Palette::MediumBackground);
	Get<SNavmeshComponent>().Walls.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(-10.f, -10.f), -1001, Palette::DarkBackground, -0.5f);
	Get<SNavmeshComponent>().Walls2.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(10.f, -10.f), -1001, Palette::DarkBackground, -0.5f);
	Get<SNavmeshComponent>().Walls3.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(-10.f, 10.f), -1001, Palette::DarkBackground, -0.5f);
	Get<SNavmeshComponent>().Walls4.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(10.f, 10.f), -1001, Palette::DarkBackground, -0.5f);
	Get<SNavmeshComponent>().Navmesh = std::move(aNavmesh);
}

void NavmeshFunctionality::Tick()
{
	if (!Get<SNavmeshComponent>().Background || !Get<SNavmeshComponent>().Walls)
		return;

	Get<SNavmeshComponent>().Walls->Render();
	Get<SNavmeshComponent>().Walls2->Render();
	Get<SNavmeshComponent>().Walls3->Render();
	Get<SNavmeshComponent>().Walls4->Render();
	Get<SNavmeshComponent>().Background->Render();
}
