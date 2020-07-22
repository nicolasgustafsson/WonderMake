#include "pch.h"
#include "NavmeshFunctionality.h"

NavmeshFunctionality::NavmeshFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void NavmeshFunctionality::SetNavmesh(Navmesh aNavmesh)
{
	Get<SNavmeshComponent>().RenderObject.emplace(aNavmesh, aNavmesh.GetTriangles().size());
	Get<SNavmeshComponent>().Navmesh = std::move(aNavmesh);
}

void NavmeshFunctionality::Tick()
{
	if (!Get<SNavmeshComponent>().RenderObject)
		return;

	Get<SNavmeshComponent>().RenderObject->Render();
}
