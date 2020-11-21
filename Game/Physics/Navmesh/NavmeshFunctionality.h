#pragma once

#include "Physics/Navmesh/Navmesh.h"
#include "Physics/Navmesh/NavmeshRenderObject.h"
#include "LineListRenderObject.h"

struct SNavmeshComponent : public SComponent
{
	Navmesh Navmesh;

	std::optional<NavmeshRenderObject> Background;
	std::optional<LineListRenderObject> WallLines;
};

class NavmeshFunctionality : public Functionality< 
	Policy::Set<
	PAdd<SNavmeshComponent, PWrite>>>
{
public:
	void SetNavmesh(Navmesh aNavmesh);

	const Navmesh& GetNavmesh() const { return Get<SNavmeshComponent>().Navmesh; };
	Navmesh& GetNavmesh() { return Get<SNavmeshComponent>().Navmesh; };

	void Tick();
};
