#include "pch.h"
#include "NavmeshFunctionality.h"
#include "Utility/Palette.h"

NavmeshFunctionality::NavmeshFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void NavmeshFunctionality::SetNavmesh(Navmesh aNavmesh)
{
	Get<SNavmeshComponent>().Background.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(0.f, 0.f), -1000, Palette::MediumBackground);
	Get<SNavmeshComponent>().Walls.emplace(aNavmesh, aNavmesh.GetTriangles().size(), SVector2f(0.f, -30.f), -1001, Palette::DarkBackground, -0.5f);
	Get<SNavmeshComponent>().Navmesh = std::move(aNavmesh);
}

void NavmeshFunctionality::Tick()
{
	if (!Get<SNavmeshComponent>().Background || !Get<SNavmeshComponent>().Walls)
		return;

	Get<SNavmeshComponent>().Walls->Render();
	Get<SNavmeshComponent>().Background->Render();
}
